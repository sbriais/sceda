/*
   This file is part of SCEDA.
   Copyright 2008, 2009 Sebastien Briais
   
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
#include "graph_negcycle.h"

#include <stdlib.h>

#undef DEBUG
//#define DEBUG

#ifdef DEBUG
#include <stdio.h>
#endif

typedef struct {
  SCEDA_int_edge_fun weight;
  void *w_ctxt;
  SCEDA_int_edge_fun time;
  void *t_ctxt;
  long double lambda;
} MRCcontext;

static long double mrc_cost(SCEDA_Edge *e, MRCcontext *ctxt) {
  int w, t;

  w = ctxt->weight(e, ctxt->w_ctxt);
  t = ctxt->time(e, ctxt->t_ctxt);

  long double res;
  res = w - ctxt->lambda * t;
  return res;
}

static int mrc_unit_cost(SCEDA_Edge *e, void *ctxt) {
  return 1;
}

#ifdef DEBUG
static void print_rational(FILE *stream, long double *x) {
  fprintf(stream,"%Lf", *x);
}
#endif

static int rational_norm(long double *x) {
  return 0;
}

static inline void rational_add(long double *x, long double *y, long double *z) {
  (*z) = (*x) + (*y);
}

static inline void rational_div_n(int n, long double *x) {
  (*x) /= n;
}

static inline void rational_int(int n, long double *x) {
  (*x) = (long double)n;
}

static inline void rational_inv_int(int n, long double *x) {
  (*x) = (1.0) / ((long double)n);
}

static inline void rational_sub(long double *x, long double *y, long double *z) {
  (*z) = (*x) - (*y);
}

static inline int rational_compare(long double *x, long double *y) {
  if((*x) < (*y)) {
    return -1;
  } else if((*x) == (*y)) {
    return 0;
  } else {
    return 1;
  }
}

static inline int small_enough(long double *min, long double *max, long double *delta) {
  long double tmp;
  rational_sub(max, min, &tmp);
  if(rational_compare(&tmp, delta) > 0) {
    return FALSE;
  } else {
    return TRUE;
  }
}

int SCEDA_graph_minimum_ratio_cycle(SCEDA_Graph *g, 
				    SCEDA_int_edge_fun weight, void *w_ctxt, 
				    SCEDA_int_edge_fun time, void *t_ctxt, 
				    int *ratio_num, int *ratio_den,
				    SCEDA_List **min_cycle) {
  if(SCEDA_graph_is_acyclic(g)) {
    *ratio_num = 0;
    *ratio_den = 0;
    return -1;
  }

  int ret_code = 0;

  int gamma = 0;
  int tau = 0;
  
  {
    int first = TRUE;
    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
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
    SCEDA_edges_iterator_cleanup(&edges);
  }

  int n = SCEDA_graph_vcount(g);

  {
    MRCcontext ctxt;
    ctxt.weight = time;
    ctxt.w_ctxt = t_ctxt;
    ctxt.time = mrc_unit_cost;
    ctxt.t_ctxt = NULL;
    
    rational_inv_int(n, &(ctxt.lambda));
    
#ifdef DEBUG
    fprintf(stdout,"checking for negative time cycles\n");
#endif

    SCEDA_List *cycle = SCEDA_graph_neg_cycle_long_double(g, (SCEDA_long_double_edge_fun)mrc_cost, &ctxt);

    if(!SCEDA_list_is_empty(cycle)) {
      ret_code = -1;
    }
    
    SCEDA_list_delete(cycle);
  }

  if(ret_code == 0) {  
    long double lambda_min;
    long double lambda_max;
    long double delta;
    
    rational_int(-n*gamma, &lambda_min);
    rational_int(n*gamma, &lambda_max);
    rational_inv_int(n*n*tau*tau, &delta);

    MRCcontext ctxt;
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
      
      SCEDA_List *cycle = SCEDA_graph_neg_cycle_long_double(g, (SCEDA_long_double_edge_fun)mrc_cost, &ctxt);
      
      if(!SCEDA_list_is_empty(cycle)) {
	lambda_max = ctxt.lambda;
      } else {
	lambda_min = ctxt.lambda;
      }

      SCEDA_list_delete(cycle);
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

      *min_cycle = SCEDA_graph_neg_cycle_long_double(g, (SCEDA_long_double_edge_fun)mrc_cost, &ctxt);

      *ratio_num = 0;
      *ratio_den = 0;
      SCEDA_ListIterator cycle_elts;
      SCEDA_list_iterator_init(*min_cycle, &cycle_elts);
      while(SCEDA_list_iterator_has_next(&cycle_elts)) {
	SCEDA_Edge *e = SCEDA_list_iterator_next(&cycle_elts);
	(*ratio_num) += weight(e, w_ctxt);
	(*ratio_den) += time(e, t_ctxt);
      }
      SCEDA_list_iterator_cleanup(&cycle_elts);
    }
  }

  return ret_code;
}
