/*
   This file is part of SCEDA.
   (c) 2008 Sebastien Briais
   
   SCEDA is free software: you can redistribute it and/or modify it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation, either version 3 of the
   License, or (at your option) any later version.
   
   SCEDA is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.
   
   You should have received a copy of the GNU Lesser General Public
   License along with SCEDA.  If not, see
   <http://www.gnu.org/licenses/>.
*/
#include "common.h"
#include "util.h"
#include "graph_dag.h"
#include "graph_mrc.h"
#include "graph_path.h"

#include <stdlib.h>

#undef DEBUG
//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

#define Rational double

#ifndef Rational
typedef struct {
  int num;
  int den;
} Rational;
#endif

typedef struct {
  SCEDA_Vertex *source;
  int (*weight)(SCEDA_Edge *e, void *ctxt);
  void *w_ctxt;
  int (*time)(SCEDA_Edge *e, void *ctxt);
  void *t_ctxt;
  Rational lambda;
} MRCcontext;

static SCEDA_COST_TYPE mrc_cost(SCEDA_Edge *e, MRCcontext *ctxt) {
  int w, t;

  if(SCEDA_edge_source(e) == ctxt->source) {
    w = 0;
    t = 0;
  } else {
    w = ctxt->weight(e, ctxt->w_ctxt);
    t = ctxt->time(e, ctxt->t_ctxt);
  }

  SCEDA_COST_TYPE res;
#ifndef Rational
  res = ctxt->lambda.den * w - ctxt->lambda.num * t;
#else
  res = w - ctxt->lambda * t;
  //  fprintf(stdout,"w = %d, t = %d => %g\n", w, t, res);
#endif
  return res;
}

static inline int max(int a, int b) {
  if(a < b) {
    return b;
  } else {
    return a;
  }
}

#ifndef Rational
#ifdef DEBUG
static void print_rational(FILE *stream, Rational *x) {
  fprintf(stream,"%d / %d", x->num, x->den);
}
#endif

static int pgcd(int a, int b) {
  if(b == 0) {
    return a;
  } else {
    return pgcd(b, a % b);
  }
}

static int rational_norm(Rational *x) {
#ifdef DEBUG
  fprintf(stdout,"normalize ");
  print_rational(stdout,x);
#endif
  int eps;
  if(((x->den < 0) && (x->num > 0)) || ((x->den > 0) && (x->num < 0))) { 
    eps = -1;
  } else {
    eps = 1;
  }
  if(x->den < 0) {
    x->den = -x->den;
  }
  if(x->num < 0) {
    x->num = -x->num;
  }

  int d = pgcd(x->num, x->den);
  x->num = x->num / d;
  x->den = x->den / d;

  if(eps == -1) {
    x->num = -x->num;
  }
#ifdef DEBUG
  fprintf(stdout," -> ");
  print_rational(stdout, x);
  fprintf(stdout,"\n");
#endif

  if(x->den <= 0) {
    return -1;
  } else {
    return 0;
  }
}

static inline void rational_add(Rational *x, Rational *y, Rational *z) {
  z->num = x->num * y->den + x->den * y->num;
  z->den = x->den * y->den;
}

static inline void rational_div_n(int n, Rational *x) {
  x->den = n*x->den;
}

static inline void rational_int(int n, Rational *x) {
  x->num = n;
  x->den = 1;
}

static inline void rational_inv_int(int n, Rational *x) {
  if(n < 0) {
    x->num = -1;
    x->den = -n;
  } else {
    x->num = 1;
    x->den = n;
  }
}

static inline void rational_sub(Rational *x, Rational *y, Rational *z) {
  z->num = x->num * y->den - x->den * y->num;
  z->den = x->den * y->den;
}

static inline int rational_compare(Rational *x, Rational *y) {
  int q1 = x->num * y->den;
  int q2 = y->num * x->den;
  if(q1 < q2) {
    return -1;
  } else if(q1 == q2) {
    return 0;
  } else {
    return 1;
  }
}
#else
#ifdef DEBUG
static void print_rational(FILE *stream, double *x) {
  fprintf(stream,"%g", *x);
}
#endif

static int rational_norm(double *x) {
  return 0;
}

static inline void rational_add(double *x, double *y, double *z) {
  (*z) = (*x) + (*y);
}

static inline void rational_div_n(int n, double *x) {
  (*x) /= n;
}

static inline void rational_int(int n, double *x) {
  (*x) = (double)n;
}

static inline void rational_inv_int(int n, double *x) {
  (*x) = (1.0) / ((double)n);
}

static inline void rational_sub(double *x, double *y, double *z) {
  (*z) = (*x) - (*y);
}

static inline int rational_compare(double *x, double *y) {
  if((*x) < (*y)) {
    return -1;
  } else if((*x) == (*y)) {
    return 0;
  } else {
    return 1;
  }
}
#endif

static inline int small_enough(Rational *min, Rational *max, Rational *delta) {
  Rational tmp;
  rational_sub(max, min, &tmp);
  if(rational_compare(&tmp, delta) > 0) {
    return FALSE;
  } else {
    return TRUE;
  }
}

int SCEDA_graph_mrc(SCEDA_Graph *g, 
		    int (*weight)(SCEDA_Edge *e, void *ctxt), void *w_ctxt, 
		    int (*time)(SCEDA_Edge *e, void *ctxt), void *t_ctxt, 
		    int *ratio_num, int *ratio_den,
		    SCEDA_List **min_cycle) {
  if(SCEDA_graph_is_acyclic(g)) {
    *ratio_num = 0;
    *ratio_den = 0;
    return -1;
  }

  int ret_code = 0;

  // add a source to the graph
  SCEDA_List *added_edges = SCEDA_list_create(NULL);
  SCEDA_Vertex *source = SCEDA_graph_add_vertex(g, NULL);
  {
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *u = SCEDA_vertices_iterator_next(&vertices);
      if(u != source) {
	SCEDA_Edge *e = SCEDA_graph_add_edge(g, source, u, NULL);
	safe_call(SCEDA_list_add(added_edges, e));
      }
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  int gamma = 0;
  int tau = 0;
  
  {
    int first = TRUE;
    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
      if(SCEDA_edge_source(e) != source) {
	int w, t;
	w = abs(weight(e, w_ctxt));
	t = abs(time(e, t_ctxt));
	if(first) {
	  gamma = w;
	  tau = t;
	  first = FALSE;
	} else {
	  if(t > tau) {
	    tau = t;
	  }
	  if(w > gamma) {
	    gamma = w;
	  }
	}
      }
    }
    SCEDA_edges_iterator_cleanup(&edges);
  }

  int n = SCEDA_graph_vcount(g);

  Rational lambda_min;
  Rational lambda_max;
  Rational delta;

  rational_int(-n*gamma, &lambda_min);
  rational_int(n*gamma, &lambda_max);
  rational_inv_int(n*n*tau*tau, &delta);

  {
    MRCcontext ctxt;
    SCEDA_Vertex *cycle;
    ctxt.source = source;
    ctxt.weight = weight;
    ctxt.w_ctxt = w_ctxt;
    ctxt.time = time;
    ctxt.t_ctxt = t_ctxt;

#ifdef DEBUG
    fprintf(stdout,"delta = ");
    print_rational(stdout,&delta);
    fprintf(stdout,"\n");
#endif
    while(!small_enough(&lambda_min, &lambda_max, &delta)) {
      rational_add(&(lambda_min), &(lambda_max), &(ctxt.lambda));
      rational_div_n(2, &(ctxt.lambda));
      if(rational_norm(&(ctxt.lambda)) != 0) {
	ret_code = -1;
	break;
      }
#ifdef DEBUG
      fprintf(stdout,"min = ");
      print_rational(stdout, &lambda_min);
      fprintf(stdout,"\tmax = ");
      print_rational(stdout, &lambda_max);
      fprintf(stdout,"\n");
      fprintf(stdout,"current = ");
      print_rational(stdout,&(ctxt.lambda));
      fprintf(stdout,"\n");
#endif
      SCEDA_HashMap *paths = SCEDA_graph_shortest_path_bellman_ford(g, source, (SCEDA_cost_fun)mrc_cost, &ctxt, &cycle);
      SCEDA_hashmap_delete(paths);
      
      if(cycle != NULL) {
	lambda_max = ctxt.lambda;
      } else {
	lambda_min = ctxt.lambda;
      }
    }

    if(ret_code == 0) {
#ifdef DEBUG
      fprintf(stdout,"min = ");
      print_rational(stdout, &lambda_min);
      fprintf(stdout,"\tmax = ");
      print_rational(stdout, &lambda_max);
      fprintf(stdout,"\n");
#endif
      ctxt.lambda = lambda_max;
#ifdef DEBUG
      fprintf(stdout,"current = ");
      print_rational(stdout,&(ctxt.lambda));
      fprintf(stdout,"\n");
#endif
      SCEDA_HashMap *paths = SCEDA_graph_shortest_path_bellman_ford(g, source, (SCEDA_cost_fun)mrc_cost, &ctxt, &cycle);
      
      safe_ptr(cycle);

      *min_cycle = SCEDA_list_create(NULL);
      
      *ratio_num = 0;
      *ratio_den = 0;
      SCEDA_Vertex *u = cycle;
      do {
	SCEDA_PathInfo *pi_u = SCEDA_hashmap_get(paths, u);
	SCEDA_Edge *in = pi_u->in_edge;
	safe_ptr(in);
	safe_call(SCEDA_list_add(*min_cycle, in));
	(*ratio_num) += weight(in, w_ctxt);
	(*ratio_den) += time(in, t_ctxt);
	u = SCEDA_edge_source(in);
      } while(u != cycle);
      
      SCEDA_hashmap_delete(paths);
    }
  }

  // restore the graph
  {
    SCEDA_ListIterator edges;
    SCEDA_list_iterator_init(added_edges, &edges);
    while(SCEDA_list_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_list_iterator_next(&edges);
      void *data;
      safe_call(SCEDA_graph_remove_edge(g, e, &data));
    }
    SCEDA_list_iterator_cleanup(&edges);
  }

  SCEDA_list_delete(added_edges);  

  void *data;
  safe_call(SCEDA_graph_remove_vertex(g, source, &data));

  return ret_code;
}
