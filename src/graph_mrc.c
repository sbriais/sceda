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
#include "list.h"

#include <stdio.h>

typedef struct {
  int num;
  int den;
} Rational;

typedef struct {
  SCEDA_Vertex *source;
  SCEDA_cost_fun weight;
  void *w_ctxt;
  SCEDA_cost_fun time;
  void *t_ctxt;
  Rational lambda;
} MRCcontext;

static int mrc_cost(SCEDA_Edge *e, MRCcontext *ctxt) {
  int w, t;

  if(SCEDA_edge_source(e) == ctxt->source) {
    w = 0;
    t = 0;
  } else {
    w = ctxt->weight(e, ctxt->w_ctxt);
    t = ctxt->time(e, ctxt->t_ctxt);
  }

  return ctxt->lambda.den * w - ctxt->lambda.num * t;
}

static int pgcd(int a, int b) {
  if(b == 0) {
    return a;
  } else {
    return pgcd(b, a % b);
  }
}

static void normalize_rational(Rational *x) {
  int eps = 1;
  if(x->den < 0) {
    x->den = -x->den;
    if(x->num < 0) {
      x->num = -x->num;
    } else {
      eps = -1;
    }
  }
  int d = pgcd(x->num, x->den);
  x->num = x->num / d;
  x->den = x->den / d;
  if(eps == -1) {
    x->num = -x->num;
  }
}

static void print_rational(FILE *stream, Rational *x) {
  fprintf(stream,"%d / %d", x->num, x->den);
}

static inline int small_enough(Rational *min, Rational *max, Rational *delta) {
  int p = max->num * min->den - min->num * max->den;
  int q = min->den * max->den;

  int x = p * delta->den;
  int y = q * delta->num;

  if(x > y) {
    return FALSE;
  } else {
    return TRUE;
  }
}

int SCEDA_graph_mrc(SCEDA_Graph *g, SCEDA_cost_fun weight, void *w_ctxt, SCEDA_cost_fun time, void *t_ctxt, int *ratio_num, int *ratio_den) {
  if(SCEDA_graph_is_acyclic(g)) {
    *ratio_num = 0;
    *ratio_den = 0;
    return -1;
  }

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

  int wmin = 0, wmax = 0;
  int tmin = 0, tmax = 0;
  
  {
    int first = TRUE;
    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
      if(SCEDA_edge_source(e) != source) {
	int w, t;
	w = weight(e, w_ctxt);
	t = time(e, t_ctxt);
	if(first) {
	  wmin = w;
	  wmax = w;
	  tmin = t;
	  tmax = t;
	  first = FALSE;
	} else {
	  if(t < tmin) {
	    tmin = t;
	  }
	  if(t > tmax) {
	    tmax = t;
	  }
	  if(w < wmin) {
	    wmin = w;
	  }
	  if(w > wmax) {
	    wmax = w;
	  }
	}
      }
    }
    SCEDA_edges_iterator_cleanup(&edges);
  }

  int n = SCEDA_graph_vcount(g);

  int gamma, tau;
  
  if((wmax < 0) || (wmax < -wmin)) {
    gamma = -wmin;
  } else {
    gamma = wmax;
  }

  if((tmax < 0) || (tmax < -tmin)) {
    tau = -tmin;
  } else {
    tau = tmax;
  }

  Rational lambda_min;
  Rational lambda_max;
  
  lambda_min.num = - n * gamma;
  lambda_min.den = 1;

  lambda_max.num = n * gamma;
  lambda_max.den = 1;

  Rational delta;
  delta.num = 1;
  delta.den = n * n * tau * tau;

  {
    MRCcontext ctxt;
    SCEDA_Vertex *cycle;
    ctxt.source = source;
    ctxt.weight = weight;
    ctxt.w_ctxt = w_ctxt;
    ctxt.time = time;
    ctxt.t_ctxt = t_ctxt;

    while(!small_enough(&lambda_min, &lambda_max, &delta)) {
      fprintf(stdout,"min = ");
      print_rational(stdout, &lambda_min);
      fprintf(stdout,"\tmax = ");
      print_rational(stdout, &lambda_max);
      fprintf(stdout,"\n");

      ctxt.lambda.num = lambda_min.num * lambda_max.den + lambda_max.num * lambda_min.den;
      ctxt.lambda.den = lambda_min.den * lambda_max.den * 2;
      normalize_rational(&(ctxt.lambda));

      SCEDA_HashMap *paths = SCEDA_graph_shortest_path_bellman_ford(g, source, (SCEDA_cost_fun)mrc_cost, &ctxt, &cycle);
      SCEDA_hashmap_delete(paths);
      if(cycle != NULL) {
	lambda_max = ctxt.lambda;
      } else {
	lambda_min = ctxt.lambda;
      }
    }

    ctxt.lambda = lambda_max;
    SCEDA_HashMap *paths = SCEDA_graph_shortest_path_bellman_ford(g, source, (SCEDA_cost_fun)mrc_cost, &ctxt, &cycle);

    safe_ptr(cycle);
    
    *ratio_num = 0;
    *ratio_den = 0;
    SCEDA_Vertex *u = cycle;
    do {
      SCEDA_PathInfo *pi_u = SCEDA_hashmap_get(paths, u);
      SCEDA_Edge *in = pi_u->in_edge;
      (*ratio_num) += weight(in, w_ctxt);
      (*ratio_den) += time(in, t_ctxt);
      u = SCEDA_edge_source(in);
    } while(u != cycle);

    SCEDA_hashmap_delete(paths);
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

  return 0;
}
