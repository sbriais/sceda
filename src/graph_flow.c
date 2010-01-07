/*
   Copyright Sebastien Briais 2008, 2009

   This file is part of SCEDA.
   
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
#include "graph_flow.h"

#include "boxed.h"
#include "queue.h"
#include "stack.h"
#include "dlist.h"

#include "util.h"

#include <stdio.h>

#define ABS(n$) \
  ({ int _n = (n$); \
     (_n>=0)?(_n):(-_n); })

#define SCEDA_graph_max_flow_highest_label SCEDA_graph_max_flow
/* #define SCEDA_graph_max_flow_relabel_to_front SCEDA_graph_max_flow */

/* #define SCEDA_augment_flow_along_neg_cycle_bellman_ford SCEDA_augment_flow_along_neg_cycle  */
#define SCEDA_augment_flow_along_neg_cycle_karp SCEDA_augment_flow_along_neg_cycle 

/* #define SCEDA_minimise_flow_cost_cycle_cancelling SCEDA_minimise_flow_cost */
#define SCEDA_minimise_flow_cost_cost_scaling_fifo SCEDA_minimise_flow_cost
/* #define SCEDA_minimise_flow_cost_cost_scaling_wave SCEDA_minimise_flow_cost */

#ifdef SCEDA_graph_max_flow_relabel_to_front
static void SCEDA_mf_height_update(SCEDA_Graph *g, SCEDA_Vertex *s, SCEDA_Vertex *t, 
				   SCEDA_int_edge_fun cap, void *cap_ctxt,
				   SCEDA_HashMap *flow, SCEDA_HashMap *height) {
  int n = SCEDA_graph_vcount(g);

  SCEDA_Queue *todo = SCEDA_queue_create(NULL);
  SCEDA_HashSet *visited = SCEDA_vertex_set_create();

  {
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *u = SCEDA_vertices_iterator_next(&vertices);
      boxed(int) h_u = SCEDA_hashmap_get(height, u);
      if(u == t) {
	safe_call(SCEDA_hashset_add(visited, t));
	safe_call(SCEDA_queue_enqueue(todo, t));
	boxed_set(h_u, 0);
      } else {
	boxed_set(h_u, n);
      }
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  while(!SCEDA_queue_is_empty(todo)) {
    SCEDA_Vertex *u;
    safe_call(SCEDA_queue_dequeue(todo, (void **)&u));
    boxed(int) h_u = SCEDA_hashmap_get(height, u);

    {
      SCEDA_VertexSuccIterator succ;
      SCEDA_vertex_succ_iterator_init(u, &succ);
      while(SCEDA_vertex_succ_iterator_has_next(&succ)) {
	SCEDA_Vertex *v = SCEDA_vertex_succ_iterator_next(&succ);
	if(SCEDA_hashset_contains(visited, v)) {
	  continue;
	}

	boxed(int) h_v = SCEDA_hashmap_get(height, v);

	SCEDA_EdgeClassIterator edges;
	SCEDA_edge_class_iterator_init(u, v, &edges);
	while(SCEDA_edge_class_iterator_has_next(&edges)) {
	  SCEDA_Edge *e = SCEDA_edge_class_iterator_next(&edges);
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  int rc = boxed_get(fe);
	  if(rc <= 0) {
	    continue;
	  }
	  boxed_set(h_v, boxed_get(h_u) + 1);
	  safe_call(SCEDA_hashset_add(visited, v));
	  safe_call(SCEDA_queue_enqueue(todo, v));
	}
	SCEDA_edge_class_iterator_cleanup(&edges);
      }
      SCEDA_vertex_succ_iterator_cleanup(&succ);
    }

    {
      SCEDA_VertexPredIterator pred;
      SCEDA_vertex_pred_iterator_init(u, &pred);
      while(SCEDA_vertex_pred_iterator_has_next(&pred)) {
	SCEDA_Vertex *v = SCEDA_vertex_pred_iterator_next(&pred);
	if(SCEDA_hashset_contains(visited, v)) {
	  continue;
	}

	boxed(int) h_v = SCEDA_hashmap_get(height, v);

	SCEDA_EdgeClassIterator edges;
	SCEDA_edge_class_iterator_init(v, u, &edges);
	while(SCEDA_edge_class_iterator_has_next(&edges)) {
	  SCEDA_Edge *e = SCEDA_edge_class_iterator_next(&edges);
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  int rc = cap(e, cap_ctxt) - boxed_get(fe);
	  if(rc <= 0) {
	    continue;
	  }
	  boxed_set(h_v, boxed_get(h_u) + 1);
	  safe_call(SCEDA_hashset_add(visited, v));
	  safe_call(SCEDA_queue_enqueue(todo, v));
	  break;
	}
	SCEDA_edge_class_iterator_cleanup(&edges);
      }
      SCEDA_vertex_pred_iterator_cleanup(&pred);
    }
  }

  boxed(int) h_s = SCEDA_hashmap_get(height, s);
  boxed_set(h_s, n);

  SCEDA_hashset_delete(visited);
  SCEDA_queue_delete(todo);
}

SCEDA_HashMap *SCEDA_graph_max_flow_relabel_to_front(SCEDA_Graph *g, SCEDA_Vertex *s, SCEDA_Vertex *t,
						     SCEDA_int_edge_fun capacity, void *c_ctxt) {
  int n = SCEDA_graph_vcount(g);

  if(s == t) {
    return NULL;
  }
  
  SCEDA_HashMap *flow = SCEDA_edge_map_create((SCEDA_delete_fun)boxed_delete);

  SCEDA_HashMap *excess = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);

  SCEDA_HashMap *height = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);

  /* Initialisation */
  {
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertices);
      SCEDA_hashmap_put(excess, v, boxed_create(int, 0), NULL);
      if(v == s) {
	SCEDA_hashmap_put(height, v, boxed_create(int, n), NULL);
      } else {
	SCEDA_hashmap_put(height, v, boxed_create(int, 0), NULL);
      }
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  {
    boxed(int) ex_s = SCEDA_hashmap_get(excess, s);

    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
      if(SCEDA_edge_source(e) == s) {
	int cap = capacity(e, c_ctxt);
	SCEDA_hashmap_put(flow, e, boxed_create(int, cap), NULL);
	SCEDA_Vertex *u = SCEDA_edge_target(e);

	boxed(int) ex_u = SCEDA_hashmap_get(excess, u);
	boxed_set(ex_u, boxed_get(ex_u) + cap);

	boxed_set(ex_s, boxed_get(ex_s) - cap);
      } else {
	SCEDA_hashmap_put(flow, e, boxed_create(int, 0), NULL);
      }
    }
    SCEDA_edges_iterator_cleanup(&edges);
  }

  SCEDA_List *todo = SCEDA_queue_create(NULL);
  {
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertices);
      if((v == s) || (v == t)) {
	continue;
      }
      boxed(int) ex_v = SCEDA_hashmap_get(excess, v);
      if(boxed_get(ex_v) > 0) {
	safe_call(SCEDA_queue_enqueue(todo, v));
      }
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  /* Preflow (relabel to front) */
  while(!SCEDA_queue_is_empty(todo)) {
    SCEDA_Vertex *u;
    safe_call(SCEDA_queue_dequeue(todo, (void **)&u));
    if(u == t) {
      continue;
    }

    boxed(int) ex_u = SCEDA_hashmap_get(excess, u);
    boxed(int) h_u = SCEDA_hashmap_get(height, u);

    /* by definition, u is exceeding, 
       since it was in the queue of exceeding vertices
       we thus discharge it */
    do {
      /* push as much flow as possible */
      {
	SCEDA_OutEdgesIterator edges;
	SCEDA_out_edges_iterator_init(u, &edges);
	while(SCEDA_out_edges_iterator_has_next(&edges)) {
	  SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&edges);
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  /* residual_capacity(u,v) = capacity(u,v) - flow(u,v) */
	  int rc = capacity(e, c_ctxt) - boxed_get(fe);
	  if(rc <= 0) {
	    continue;
	  }
	  SCEDA_Vertex *v = SCEDA_edge_target(e);
	  boxed(int) h_v = SCEDA_hashmap_get(height, v);
	  if(boxed_get(h_v) < boxed_get(h_u)) {
	    /* h(u) = h(v) + 1 so we can push */
	    /* amount of flow to push */
	    int push = boxed_get(ex_u);
	    if(push > rc) {
	      /* cannot push more than the residual capacity */
	      push = rc;
	    }
	    /* update excess values */
	    boxed_set(fe, boxed_get(fe) + push);
	    boxed(int) ex_v = SCEDA_hashmap_get(excess, v);
	    boxed_set(ex_u, boxed_get(ex_u) - push);
	    boxed_set(ex_v, boxed_get(ex_v) + push);
	    if((boxed_get(ex_v) > 0) && (boxed_get(ex_v) <= push)) {
	      /* v was not exceeding (hence not in the queue)
		 but is now exceeding */
	      safe_call(SCEDA_queue_enqueue(todo, v));
	    }
	    if(boxed_get(ex_u) == 0) {
	      /* nothing to push anymore */
	      break;
	    }
	  }
	}
	SCEDA_out_edges_iterator_cleanup(&edges);
      }

      /* push back as much flow as possible */
      if(boxed_get(ex_u) > 0) {
	/* this is essentially the same algorithm as above
	   but residual capacity is computed differently */
	SCEDA_InEdgesIterator edges;
	SCEDA_in_edges_iterator_init(u, &edges);
	while(SCEDA_in_edges_iterator_has_next(&edges)) {
	  SCEDA_Edge *e = SCEDA_in_edges_iterator_next(&edges);
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  if(boxed_get(fe) <= 0) {
	    continue;
	  }
	  SCEDA_Vertex *v = SCEDA_edge_source(e);
	  boxed(int) h_v = SCEDA_hashmap_get(height, v);
	  if(boxed_get(h_v) < boxed_get(h_u)) {
	    int push = boxed_get(ex_u);
	    if(push > boxed_get(fe)) {
	      /* cannot push back more than the flow */
	      push = boxed_get(fe);
	    }
	    boxed_set(fe, boxed_get(fe) - push);
	    boxed(int) ex_v = SCEDA_hashmap_get(excess, v);
	    boxed_set(ex_u, boxed_get(ex_u) - push);
	    boxed_set(ex_v, boxed_get(ex_v) + push);
	    if((boxed_get(ex_v) > 0) && (boxed_get(ex_v) <= push)) {
	      /* v was not exceeding (hence not in the queue)
		 but is now exceeding */
	      safe_call(SCEDA_queue_enqueue(todo, v));
	    }
	    if(boxed_get(ex_u) == 0) {
	      /* nothing to push anymore */
	      break;
	    }
	  }
	}
	SCEDA_in_edges_iterator_cleanup(&edges);
      }
    
      /* if u is still exceeding, increase its height (relabel) */
      if(boxed_get(ex_u) > 0) {
	boxed_set(h_u, boxed_get(h_u) + 1);
      }
    } while(boxed_get(ex_u) > 0);
  }
  
  /* Finished */
  
  SCEDA_queue_delete(todo);
  
  SCEDA_hashmap_delete(height);
  SCEDA_hashmap_delete(excess);
  
  return flow;
}
#endif

#ifdef SCEDA_graph_max_flow_highest_label
SCEDA_HashMap *SCEDA_graph_max_flow_highest_label(SCEDA_Graph *g, SCEDA_Vertex *s, SCEDA_Vertex *t,
						  SCEDA_int_edge_fun capacity, void *c_ctxt) {
  if(s == t) {
    return NULL;
  }

  int n = SCEDA_graph_vcount(g);
  
  SCEDA_HashMap *flow = SCEDA_edge_map_create((SCEDA_delete_fun)boxed_delete);

  SCEDA_HashMap *excess = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);

  SCEDA_HashMap *height = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);

  /* Initialisation */
  {
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertices);
      SCEDA_hashmap_put(excess, v, boxed_create(int, 0), NULL);
      if(v == s) {
	SCEDA_hashmap_put(height, v, boxed_create(int, n), NULL);
      } else {
	SCEDA_hashmap_put(height, v, boxed_create(int, 0), NULL);
      }
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  {
    boxed(int) ex_s = SCEDA_hashmap_get(excess, s);

    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
      if(SCEDA_edge_source(e) == s) {
	int cap = capacity(e, c_ctxt);
	SCEDA_hashmap_put(flow, e, boxed_create(int, cap), NULL);
	SCEDA_Vertex *u = SCEDA_edge_target(e);

	boxed(int) ex_u = SCEDA_hashmap_get(excess, u);
	boxed_set(ex_u, boxed_get(ex_u) + cap);

	boxed_set(ex_s, boxed_get(ex_s) - cap);
      } else {
	SCEDA_hashmap_put(flow, e, boxed_create(int, 0), NULL);
      }
    }
    SCEDA_edges_iterator_cleanup(&edges);
  }

  SCEDA_Queue levels[2*n];
  {
    int i;
    for(i = 0; i < 2 * n; i++) {
      SCEDA_queue_init(&levels[i], NULL);
    }
  }

  {
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertices);
      if((v == s) || (v == t)) {
	continue;
      }
      boxed(int) ex_v = SCEDA_hashmap_get(excess, v);
      if(boxed_get(ex_v) > 0) {
	safe_call(SCEDA_queue_enqueue(&levels[0], v));
      }
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  int highest = 0;

  /* Preflow (highest label) */
  while(highest >= 0) {
    if(SCEDA_queue_is_empty(&levels[highest])) {
      highest--;
      continue;
    }

    SCEDA_Vertex *u;
    safe_call(SCEDA_queue_dequeue(&levels[highest], (void **)&u));
    if(u == t) {
      continue;
    }

    boxed(int) ex_u = SCEDA_hashmap_get(excess, u);
    boxed(int) h_u = SCEDA_hashmap_get(height, u);

    /* by definition, u is exceeding, 
       since it was in the queue of exceeding vertices
       we thus discharge it */
    do {
      /* push as much flow as possible */
      {
	SCEDA_OutEdgesIterator edges;
	SCEDA_out_edges_iterator_init(u, &edges);
	while(SCEDA_out_edges_iterator_has_next(&edges)) {
	  SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&edges);
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  /* residual_capacity(u,v) = capacity(u,v) - flow(u,v) */
	  int rc = capacity(e, c_ctxt) - boxed_get(fe);
	  if(rc <= 0) {
	    continue;
	  }
	  SCEDA_Vertex *v = SCEDA_edge_target(e);
	  boxed(int) h_v = SCEDA_hashmap_get(height, v);
	  if(boxed_get(h_v) < boxed_get(h_u)) {
	    /* h(u) = h(v) + 1 so we can push */
	    /* amount of flow to push */
	    int push = boxed_get(ex_u);
	    if(push > rc) {
	      /* cannot push more than the residual capacity */
	      push = rc;
	    }
	    /* update flow & excess values */
	    boxed_set(fe, boxed_get(fe) + push);
	    boxed(int) ex_v = SCEDA_hashmap_get(excess, v);
	    boxed_set(ex_u, boxed_get(ex_u) - push);
	    boxed_set(ex_v, boxed_get(ex_v) + push);
	    if((boxed_get(ex_v) > 0) && (boxed_get(ex_v) <= push)) {
	      /* v was not exceeding (hence not in the queue)
		 but is now exceeding */
	      safe_call(SCEDA_queue_enqueue(&levels[boxed_get(h_v)], v));
	    }
	    if(boxed_get(ex_u) == 0) {
	      /* nothing to push anymore */
	      break;
	    }
	  }
	}
	SCEDA_out_edges_iterator_cleanup(&edges);
      }

      /* push back as much flow as possible */
      if(boxed_get(ex_u) > 0) {
	/* this is essentially the same algorithm as above
	   but residual capacity is computed differently */
	SCEDA_InEdgesIterator edges;
	SCEDA_in_edges_iterator_init(u, &edges);
	while(SCEDA_in_edges_iterator_has_next(&edges)) {
	  SCEDA_Edge *e = SCEDA_in_edges_iterator_next(&edges);
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  if(boxed_get(fe) <= 0) {
	    continue;
	  }
	  SCEDA_Vertex *v = SCEDA_edge_source(e);
	  boxed(int) h_v = SCEDA_hashmap_get(height, v);
	  if(boxed_get(h_v) < boxed_get(h_u)) {
	    /* h(u) = h(v) + 1 so we can push */
	    /* amount of flow to push */
	    int push = boxed_get(ex_u);
	    if(push > boxed_get(fe)) {
	      /* cannot push back more than the flow */
	      push = boxed_get(fe);
	    }
	    /* update flow & excess values */
	    boxed_set(fe, boxed_get(fe) - push);
	    boxed(int) ex_v = SCEDA_hashmap_get(excess, v);
	    boxed_set(ex_u, boxed_get(ex_u) - push);
	    boxed_set(ex_v, boxed_get(ex_v) + push);
	    if((boxed_get(ex_v) > 0) && (boxed_get(ex_v) <= push)) {
	      /* v was not exceeding (hence not in the queue)
		 but is now exceeding */
	      safe_call(SCEDA_queue_enqueue(&levels[boxed_get(h_v)], v));
	    }
	    if(boxed_get(ex_u) == 0) {
	      /* nothing to push anymore */
	      break;
	    }
	  }
	}
	SCEDA_in_edges_iterator_cleanup(&edges);
      }
    
      /* if u is still exceeding, increase its height (relabel) */
      if(boxed_get(ex_u) > 0) {
	boxed_set(h_u, boxed_get(h_u) + 1);
	highest++;
      }
    } while(boxed_get(ex_u) > 0);
  }
  
  /* Finished */
  
  {
    int i;
    for(i = 0; i < 2*n; i++) {
      SCEDA_queue_cleanup(&levels[i]);
    }
  }
  
  SCEDA_hashmap_delete(height);
  SCEDA_hashmap_delete(excess);
  
  return flow;
}
#endif

#ifdef SCEDA_minimise_flow_cost_cycle_cancelling

#ifdef SCEDA_augment_flow_along_neg_cycle_bellman_ford
static int SCEDA_augment_flow_along_neg_cycle_bellman_ford(SCEDA_Graph *g, 
							   SCEDA_int_edge_fun capacity, void *cap_ctxt, 
							   SCEDA_int_edge_fun cost, void *cost_ctxt, 
							   SCEDA_HashMap *flow) {
  int n = SCEDA_graph_vcount(g);

  SCEDA_HashMap *incoming_edges = SCEDA_vertex_map_create(NULL);
  SCEDA_HashMap *reversed = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);
  SCEDA_HashMap *dist = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);

  SCEDA_HashSet *in_queue = SCEDA_vertex_set_create();
  SCEDA_Queue *queue = SCEDA_queue_create(NULL);

  /* Initially, each node is at distance 0 from a virtual source */
  {
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertices);
      safe_call(SCEDA_hashmap_put(dist, v, boxed_create(int, 0), NULL));
      safe_call(SCEDA_hashmap_put(reversed, v, boxed_create(int, FALSE), NULL));
      safe_call(SCEDA_queue_enqueue(queue, v));
      safe_call(SCEDA_hashset_add(in_queue, v));  
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  safe_call(SCEDA_queue_enqueue(queue, NULL));

  /* This is an adaptation of Bellman-Ford algorithm, that works on the residual graph */
  int i = 0;
  while((i < n) && (!SCEDA_queue_is_empty(queue))) {
    SCEDA_Vertex *u;
    safe_call(SCEDA_queue_dequeue(queue, (void **)&u));
    if(u == NULL) {
      i++;
      if(i < n) {
	safe_call(SCEDA_queue_enqueue(queue, NULL));
      }
      continue;
    }
    safe_call(SCEDA_hashset_remove(in_queue, (void **)&u));
    
    boxed(int) dist_u = SCEDA_hashmap_get(dist, u);
   
    /* we first iterate over out edges with positive residual capacity */
    SCEDA_OutEdgesIterator out_edges;
    SCEDA_out_edges_iterator_init(u, &out_edges);
    while(SCEDA_out_edges_iterator_has_next(&out_edges)) {
      SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&out_edges);
      boxed(int) fe = SCEDA_hashmap_get(flow, e);
      int rc = capacity(e, cap_ctxt) - boxed_get(fe);
      if(rc <= 0) {
	continue;
      }
      SCEDA_Vertex *v = SCEDA_edge_target(e);
      boxed(int) dist_v = SCEDA_hashmap_get(dist, v);
      boxed(int) rev_v = SCEDA_hashmap_get(reversed, v);
      int ce = cost(e, cost_ctxt);
      if(boxed_get(dist_u) + ce < boxed_get(dist_v)) {
	SCEDA_hashmap_put(incoming_edges, v, e, NULL);
	boxed_set(dist_v, boxed_get(dist_u) + ce);
	boxed_set(rev_v, FALSE);
	if(!SCEDA_hashset_contains(in_queue, v)) {
	  safe_call(SCEDA_queue_enqueue(queue, v));
	  safe_call(SCEDA_hashset_add(in_queue, v));
	}
      }
    }
    SCEDA_out_edges_iterator_cleanup(&out_edges);
    
    /* then we iterate over in edges with positive residual capacity (flow) */
    SCEDA_InEdgesIterator in_edges;
    SCEDA_in_edges_iterator_init(u, &in_edges);
    while(SCEDA_in_edges_iterator_has_next(&in_edges)) {
      SCEDA_Edge *e = SCEDA_in_edges_iterator_next(&in_edges);
      boxed(int) fe = SCEDA_hashmap_get(flow, e);
      if(boxed_get(fe) <= 0) {
	continue;
      }
      SCEDA_Vertex *v = SCEDA_edge_source(e);
      boxed(int) dist_v = SCEDA_hashmap_get(dist, v);
      boxed(int) rev_v = SCEDA_hashmap_get(reversed, v);
      int ce = -cost(e, cost_ctxt);
      if(boxed_get(dist_u) + ce < boxed_get(dist_v)) {
	SCEDA_hashmap_put(incoming_edges, v, e, NULL);
	boxed_set(dist_v, boxed_get(dist_u) + ce);
	boxed_set(rev_v, TRUE);
	if(!SCEDA_hashset_contains(in_queue, v)) {
	  safe_call(SCEDA_queue_enqueue(queue, v));
	  safe_call(SCEDA_hashset_add(in_queue, v));
	}
      }
    }
    SCEDA_in_edges_iterator_cleanup(&in_edges);
  }

  SCEDA_hashmap_delete(dist);
  SCEDA_hashset_delete(in_queue);

  if((i < n) || (SCEDA_queue_is_empty(queue))) {
    SCEDA_queue_delete(queue);
    SCEDA_hashmap_delete(incoming_edges);
    SCEDA_hashmap_delete(reversed);
    return FALSE;
  }

  /* We can augment the flow along a negative cost cycle ! */
  SCEDA_Vertex *cycle;
  safe_call(SCEDA_queue_dequeue(queue, (void **)&cycle));
  SCEDA_queue_delete(queue);

  {
    SCEDA_HashSet *cycle_elts = SCEDA_vertex_set_create();
    do {
      safe_call(SCEDA_hashset_add(cycle_elts, cycle));
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edges, cycle);
      safe_ptr(e);
      boxed(int) rev_e = SCEDA_hashmap_get(reversed, cycle);
      if(boxed_get(rev_e)) {
	cycle = SCEDA_edge_target(e);
      } else {
	cycle = SCEDA_edge_source(e);
      }
    } while(!SCEDA_hashset_contains(cycle_elts, cycle));
    SCEDA_hashset_delete(cycle_elts);
  }

  int min_flow = -1;

  {
    SCEDA_Vertex *u = cycle;
    do {
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edges, u);
      boxed(int) rev_e = SCEDA_hashmap_get(reversed, u);
      boxed(int) fe = SCEDA_hashmap_get(flow, e);
      SCEDA_Vertex *v;
      int rc;
      if(boxed_get(rev_e)) {
	v = SCEDA_edge_target(e);
	rc = boxed_get(fe);
      } else {
	v = SCEDA_edge_source(e);
	rc = capacity(e, cap_ctxt) - boxed_get(fe);
      }
      if((min_flow == -1) || (rc < min_flow)) {
	min_flow = rc;
      }
      u = v;
    } while(u != cycle);
  }
  
  {
    SCEDA_Vertex *u = cycle;
    do {
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edges, u);
      boxed(int) rev_e = SCEDA_hashmap_get(reversed, u);
      boxed(int) fe = SCEDA_hashmap_get(flow, e);
      SCEDA_Vertex *v;
      if(boxed_get(rev_e)) {
	v = SCEDA_edge_target(e);
	boxed_set(fe, boxed_get(fe) - min_flow);
      } else {
	v = SCEDA_edge_source(e);
	boxed_set(fe, boxed_get(fe) + min_flow);
      }
      u = v;
    } while(u != cycle);
  }

  SCEDA_hashmap_delete(incoming_edges);
  SCEDA_hashmap_delete(reversed);

  return TRUE;
}
#endif

#ifdef SCEDA_augment_flow_along_neg_cycle_karp
static int SCEDA_augment_flow_along_neg_cycle_karp(SCEDA_Graph *g, 
						   SCEDA_int_edge_fun capacity, void *cap_ctxt, 
						   SCEDA_int_edge_fun cost, void *cost_ctxt, 
						   SCEDA_HashMap *flow) {
  int n = SCEDA_graph_vcount(g);

  SCEDA_HashMap *dist[n+1];
  SCEDA_HashMap *incoming_edge[n+1];
  SCEDA_HashMap *reversed[n+1]; 

  {
    int i;
    for(i = 0; i < n+1; i++) {
      dist[i] = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);
      incoming_edge[i] = SCEDA_vertex_map_create(NULL);
      reversed[i] = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);
    }
  }
  
  /** This is an adaptation of Karp mean cycle algorithm */
  /** that works on the residual graph */

  /* initialisation */
  /* dist[0] = distance from "virtual source" to each vertex in exactly one (virtual) edge */
  {
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertices);
      SCEDA_hashmap_put(dist[0], v, boxed_create(int, 0), NULL);
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  {
    int i;
    for(i = 1; i < n+1; i++) {
      SCEDA_VerticesIterator vertices;
      SCEDA_vertices_iterator_init(g, &vertices);
      while(SCEDA_vertices_iterator_has_next(&vertices)) {
	SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertices);
	
	boxed(int) dv = boxed_create(int, 0);
	SCEDA_Edge *in_v = NULL;
	int rev = FALSE;

	/** Iterate over input edges in the residual graph */
	{
	  /** First iterate over the reversed output edges with phi > 0 */
	  SCEDA_OutEdgesIterator out_edges;
	  SCEDA_out_edges_iterator_init(v, &out_edges);
	  while(SCEDA_out_edges_iterator_has_next(&out_edges)) {
	    SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&out_edges);
	    /* Check residual capacity */
	    boxed(int) fe = SCEDA_hashmap_get(flow, e);
	    if(boxed_get(fe) <= 0) {
	      continue;
	    }

	    SCEDA_Vertex *u = SCEDA_edge_target(e);

	    boxed(int) du = SCEDA_hashmap_get(dist[i-1], u);
	    if(du == NULL) {
	      continue;
	    }

	    int ce = -cost(e, cost_ctxt);

	    if((in_v == NULL) || (boxed_get(du) + ce < boxed_get(dv))) {
	      in_v = e;
	      rev = TRUE;
	      boxed_set(dv, boxed_get(du) + ce);
	    }
	  }
	  SCEDA_out_edges_iterator_cleanup(&out_edges);
	  
	  /** Then iterate over the input edges with rc > 0 */
	  SCEDA_InEdgesIterator in_edges;
	  SCEDA_in_edges_iterator_init(v, &in_edges);
	  while(SCEDA_in_edges_iterator_has_next(&in_edges)) {
	    SCEDA_Edge *e = SCEDA_in_edges_iterator_next(&in_edges);
	    /* Check residual capacity */
	    boxed(int) fe = SCEDA_hashmap_get(flow, e);
	    int rc = capacity(e, cap_ctxt) - boxed_get(fe);
	    if(rc <= 0) {
	      continue;
	    }
	    
	    SCEDA_Vertex *u = SCEDA_edge_source(e);
	    
	    boxed(int) du = SCEDA_hashmap_get(dist[i-1],u);
	    if(du == NULL) {
	      continue;
	    }
	    
	    int ce = cost(e, cost_ctxt);
	    
	    if((in_v == NULL) || (boxed_get(du) + ce < boxed_get(dv))) {
	      in_v = e;
	      rev = FALSE;
	      boxed_set(dv, boxed_get(du) + ce);
	    }
	  }
	  SCEDA_in_edges_iterator_cleanup(&in_edges);
	}

	if(in_v != NULL) {
	  SCEDA_hashmap_put(dist[i], v, dv, NULL);
	  SCEDA_hashmap_put(incoming_edge[i], v, in_v, NULL);
	  SCEDA_hashmap_put(reversed[i], v, boxed_create(int, rev), NULL);
	} else {
	  boxed_delete(dv);
	}
      }
      SCEDA_vertices_iterator_cleanup(&vertices);
    }
  }

  SCEDA_Vertex *mu_v = NULL;
  int mu_num = 0;
  int mu_den = 0;

  {
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertices);

      boxed(int) dv_n = SCEDA_hashmap_get(dist[n], v);
      if(dv_n == NULL) {
	continue;
      }

      int num_max = 0;
      int den_max = 0;

      int k;
      for(k = 0; k < n; k++) {
	boxed(int) dv_k = SCEDA_hashmap_get(dist[k], v);
	if(dv_k == NULL) {
	  continue;
	}

	int num = boxed_get(dv_n) - boxed_get(dv_k);
	int den = n-k; /* actually (n+1) - (k+1) */

	if((den_max == 0) || (num_max * den < den_max * num)) {
	  num_max = num;
	  den_max = den;
	} 
      }

      if((mu_v == NULL) || (num_max * mu_den < den_max * mu_num)) {
	mu_v = v;
	mu_num = num_max;
	mu_den = den_max;
      }
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  /** Is there a cycle of negative min mean cost ? */
  if((mu_v != NULL) && (mu_num < 0)) {
    SCEDA_HashSet *in_cycle = SCEDA_vertex_set_create();
    SCEDA_Vertex *cycle = mu_v;
    int k = n;
    do {
      SCEDA_hashset_add(in_cycle, cycle);
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edge[k], cycle);
      boxed(int) rev = SCEDA_hashmap_get(reversed[k], cycle);
      safe_ptr(e);
      if(boxed_get(rev)) {
	cycle = SCEDA_edge_target(e);
      } else {
	cycle = SCEDA_edge_source(e);
      }
      k--;
    } while(!SCEDA_hashset_contains(in_cycle, cycle));
    SCEDA_hashset_delete(in_cycle);
    
    SCEDA_Vertex *v = mu_v;
    k = n;
    while(v != cycle) {
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edge[k], v);
      boxed(int) rev = SCEDA_hashmap_get(reversed[k], v);
      if(boxed_get(rev)) {
	v = SCEDA_edge_target(e);
      } else {
	v = SCEDA_edge_source(e);
      }
      k--;
    }
    
    int kcycle = k;
    int min_flow = -1;
    do {
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edge[k], v);
      boxed(int) rev = SCEDA_hashmap_get(reversed[k], v);
      boxed(int) fe = SCEDA_hashmap_get(flow, e);
      int rc;
      if(boxed_get(rev)) {
	v = SCEDA_edge_target(e);
	rc = boxed_get(fe);
      } else {
	v = SCEDA_edge_source(e);
	rc = capacity(e, cap_ctxt) - boxed_get(fe);
      }
      if((min_flow == -1) || (rc < min_flow)) {
	min_flow = rc;
      }
      k--;
    } while(v != cycle);

    k = kcycle;
    do {
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edge[k], v);
      boxed(int) rev = SCEDA_hashmap_get(reversed[k], v);
      boxed(int) fe = SCEDA_hashmap_get(flow, e);
      if(boxed_get(rev)) {
	v = SCEDA_edge_target(e);
	boxed_set(fe, boxed_get(fe) - min_flow);
      } else {
	v = SCEDA_edge_source(e);
	boxed_set(fe, boxed_get(fe) + min_flow);
      }
      k--;
    } while(v != cycle);
  }

  {
    int i;
    for(i = 0; i < n+1; i++) {
      SCEDA_hashmap_delete(dist[i]);
      SCEDA_hashmap_delete(incoming_edge[i]);
      SCEDA_hashmap_delete(reversed[i]);
    }
  }

  if((mu_v != NULL) && (mu_num < 0)) {
    return TRUE;
  } else {
    return FALSE;
  }
}
#endif

static void SCEDA_minimise_flow_cost_cycle_cancelling(SCEDA_Graph *g, 
						      SCEDA_int_edge_fun cap, void *cap_ctxt, 
						      SCEDA_int_edge_fun cost, void *cost_ctxt, 
						      SCEDA_HashMap *flow) {
  while(SCEDA_augment_flow_along_neg_cycle(g, cap, cap_ctxt, cost, cost_ctxt, flow)) {
  }
}
#endif

#ifdef SCEDA_minimise_flow_cost_cost_scaling_fifo
static void SCEDA_mcf_price_update(SCEDA_Graph *g,
				   SCEDA_int_edge_fun cap, void *cap_ctxt, 
				   SCEDA_int_edge_fun cost, void *cost_ctxt, 
				   SCEDA_HashMap *flow,
				   SCEDA_HashMap *excess, SCEDA_HashMap *pi, double epsilon) {
  SCEDA_Queue *todo = SCEDA_queue_create(NULL);
  SCEDA_HashSet *visited = SCEDA_vertex_set_create();
  SCEDA_HashSet *compl = SCEDA_vertex_set_create();

  /* Perform a backward breadth first traversal in the admissible
     graph from nodes with negative excess */
  {
    /** Initialise the seeds */
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *u = SCEDA_vertices_iterator_next(&vertices);
      boxed(int) eu = SCEDA_hashmap_get(excess, u);
      if(boxed_get(eu) <= 0) {
	safe_call(SCEDA_hashset_add(visited, u));
	safe_call(SCEDA_queue_enqueue(todo, u));
      } else {
	safe_call(SCEDA_hashset_add(compl, u));
      }
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  /** Perform the (backward) reachability analysis in the admissible graph */
  while(!SCEDA_queue_is_empty(todo)) {
    SCEDA_Vertex *u;
    safe_call(SCEDA_queue_dequeue(todo, (void **)&u));
    
    boxed(double) piu = SCEDA_hashmap_get(pi, u);
    {
      SCEDA_VertexSuccIterator succ;
      SCEDA_vertex_succ_iterator_init(u, &succ);
      while(SCEDA_vertex_succ_iterator_has_next(&succ)) {
	SCEDA_Vertex *v = SCEDA_vertex_succ_iterator_next(&succ);
	if(SCEDA_hashset_contains(visited, v)) {
	  continue;
	}
	
	boxed(double) piv = SCEDA_hashmap_get(pi, v);

	SCEDA_EdgeClassIterator edges;
	SCEDA_edge_class_iterator_init(u, v, &edges);
	while(SCEDA_edge_class_iterator_has_next(&edges)) {
	  SCEDA_Edge *e = SCEDA_edge_class_iterator_next(&edges);
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  int rc = boxed_get(fe);
	  if(rc <= 0) {
	    continue;
	  }
	  double ce_red = -cost(e, cost_ctxt) - boxed_get(piv) + boxed_get(piu);
	  if((-epsilon <= ce_red) && (ce_red < 0)) {
	    safe_call(SCEDA_hashset_add(visited, v));
	    safe_call(SCEDA_queue_enqueue(todo, v));
	    safe_call(SCEDA_hashset_remove(compl, (void **)&v));
	    break;
	  }
	}
	SCEDA_edge_class_iterator_cleanup(&edges);
      }
      SCEDA_vertex_succ_iterator_cleanup(&succ);
    }

    {
      SCEDA_VertexPredIterator pred;
      SCEDA_vertex_pred_iterator_init(u, &pred);
      while(SCEDA_vertex_pred_iterator_has_next(&pred)) {
	SCEDA_Vertex *v = SCEDA_vertex_pred_iterator_next(&pred);
	if(SCEDA_hashset_contains(visited, v)) {
	  continue;
	}

	boxed(double) piv = SCEDA_hashmap_get(pi, v);
	
	SCEDA_EdgeClassIterator edges;
	SCEDA_edge_class_iterator_init(v, u, &edges);
	while(SCEDA_edge_class_iterator_has_next(&edges)) {
	  SCEDA_Edge *e = SCEDA_edge_class_iterator_next(&edges);
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  int rc = cap(e, cap_ctxt) - boxed_get(fe);
	  if(rc <= 0) {
	    continue;
	  }
	  double ce_red = cost(e, cost_ctxt) - boxed_get(piv) + boxed_get(piu);
	  if((-epsilon <= ce_red) && (ce_red < 0)) {
	    safe_call(SCEDA_hashset_add(visited, v));
	    safe_call(SCEDA_queue_enqueue(todo, v));
	    safe_call(SCEDA_hashset_remove(compl, (void **)&v));
	    break;
	  }
	}
	SCEDA_edge_class_iterator_cleanup(&edges);
      }
      SCEDA_vertex_pred_iterator_cleanup(&pred);
    }
  }

  {
    SCEDA_HashSetIterator vertices;
    SCEDA_hashset_iterator_init(compl, &vertices);
    while(SCEDA_hashset_iterator_has_next(&vertices)) {
      SCEDA_Vertex *u = SCEDA_hashset_iterator_next(&vertices);
      boxed(double) piu = SCEDA_hashmap_get(pi, u);
      boxed_set(piu, boxed_get(piu) + epsilon);
    }
    SCEDA_hashset_iterator_cleanup(&vertices);
  }

  SCEDA_hashset_delete(compl);
  SCEDA_hashset_delete(visited);
  SCEDA_queue_delete(todo);
}

static void SCEDA_minimise_flow_cost_cost_scaling_fifo(SCEDA_Graph *g, 
						       SCEDA_int_edge_fun cap, void *cap_ctxt, 
						       SCEDA_int_edge_fun cost, void *cost_ctxt, 
						       SCEDA_HashMap *flow) {
  int n = SCEDA_graph_vcount(g);

  SCEDA_HashMap *excess = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);

  SCEDA_HashMap *pi = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);

  double epsilon = 1;

  /* Initialise excess, pi, and epsilon */
  {
    /** we have excess(v) = supply(v) + sum(e \in In(v)) f(e) - sum(e \in Out(v)) f(e) */
    /** since f is a flow, excess(v) = 0 for any v */
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *u = SCEDA_vertices_iterator_next(&vertices);
      safe_call(SCEDA_hashmap_put(excess, u, boxed_create(int, 0), NULL));
      safe_call(SCEDA_hashmap_put(pi, u, boxed_create(double, 0), NULL));
    }
    SCEDA_vertices_iterator_cleanup(&vertices);

    int C = 0;

    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
      int ce = cost(e, cost_ctxt);
      if(ABS(ce) > C) {
	C = ABS(ce);
      }
    }
    SCEDA_edges_iterator_cleanup(&edges);

    epsilon = 1;
    while(epsilon < C) {
      epsilon *= 2;
    }
  }

  while(n * epsilon >= 1) {
    {
      epsilon = epsilon / 2;

      SCEDA_EdgesIterator edges;
      SCEDA_edges_iterator_init(g, &edges);
      while(SCEDA_edges_iterator_has_next(&edges)) {
	SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
	int ce = cost(e, cost_ctxt);
	SCEDA_Vertex *u = SCEDA_edge_source(e);
	SCEDA_Vertex *v = SCEDA_edge_target(e);
	double ce_red = ce - boxed_get(((boxed(double))SCEDA_hashmap_get(pi, u))) + boxed_get(((boxed(double))SCEDA_hashmap_get(pi, v)));
	if(ce_red > 0) {
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  boxed(int) eu = SCEDA_hashmap_get(excess, u);
	  boxed(int) ev = SCEDA_hashmap_get(excess, v);
	  boxed_set(eu, boxed_get(eu) + boxed_get(fe));
	  boxed_set(ev, boxed_get(ev) - boxed_get(fe));
	  boxed_set(fe, 0);
	} else if(ce_red < 0) {
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  int ucap = cap(e, cap_ctxt);
	  boxed(int) eu = SCEDA_hashmap_get(excess, u);
	  boxed(int) ev = SCEDA_hashmap_get(excess, v);
	  boxed_set(eu, boxed_get(eu) + boxed_get(fe) - ucap);
	  boxed_set(ev, boxed_get(ev) - boxed_get(fe) + ucap);
	  boxed_set(fe, ucap);
	}
      }
      SCEDA_edges_iterator_cleanup(&edges);
    }

    SCEDA_Queue *todo = SCEDA_queue_create(NULL);

    {
      SCEDA_VerticesIterator vertices;
      SCEDA_vertices_iterator_init(g, &vertices);
      while(SCEDA_vertices_iterator_has_next(&vertices)) {
	SCEDA_Vertex *u = SCEDA_vertices_iterator_next(&vertices);
	boxed(int) eu = SCEDA_hashmap_get(excess, u);
	if(boxed_get(eu) > 0) {
	  safe_call(SCEDA_queue_enqueue(todo, u));
	}
      }
      SCEDA_vertices_iterator_cleanup(&vertices);
    }

    int relabelling = 0;

    while(!SCEDA_queue_is_empty(todo)) {
      if(relabelling > n) {
	relabelling = 0;
	SCEDA_mcf_price_update(g, cap, cap_ctxt, cost, cost_ctxt, flow, excess, pi, epsilon);
      }

      SCEDA_Vertex *u;
      safe_call(SCEDA_queue_dequeue(todo, (void **)&u));
 
      boxed(int) eu = SCEDA_hashmap_get(excess, u);
      boxed(double) piu = SCEDA_hashmap_get(pi, u);
      do {
	double ce_red_min = -1;

	{
	  SCEDA_OutEdgesIterator out_edges;
	  SCEDA_out_edges_iterator_init(u, &out_edges);
	  while(SCEDA_out_edges_iterator_has_next(&out_edges)) {
	    SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&out_edges);
	    boxed(int) fe = SCEDA_hashmap_get(flow, e);
	    int rc = cap(e, cap_ctxt) - boxed_get(fe);
	    /** check whether e is in the residual graph */
	    if(rc <= 0) {
	      continue;
	    }
	    SCEDA_Vertex *v = SCEDA_edge_target(e);
	    boxed(double) piv = SCEDA_hashmap_get(pi, v);
	    double ce_red = cost(e, cost_ctxt) - boxed_get(piu) + boxed_get(piv);
	    /** check whether e is an admissible edge */
	    if((-epsilon <= ce_red) && (ce_red < 0)) {
	      int push = boxed_get(eu);
	      if(push > rc) {
		push = rc;
	      }
	      boxed_set(fe, boxed_get(fe) + push);

	      boxed(int) ev = SCEDA_hashmap_get(excess, v);
	      boxed_set(eu, boxed_get(eu) - push);
	      boxed_set(ev, boxed_get(ev) + push);
	      if((boxed_get(ev) > 0) && (boxed_get(ev) <= push)) {
		safe_call(SCEDA_queue_enqueue(todo, v)); 
	      }
	      if(boxed_get(eu) == 0) {
		break;
	      }
	    } else {
	      if(ce_red >= 0) {
		if((ce_red_min < 0) || (ce_red < ce_red_min)) {
		  ce_red_min = ce_red;
		} 
	      }
	    }
	  }
	  SCEDA_out_edges_iterator_cleanup(&out_edges);
	}

	if(boxed_get(eu) > 0) {
	  SCEDA_InEdgesIterator in_edges;
	  SCEDA_in_edges_iterator_init(u, &in_edges);
	  while(SCEDA_in_edges_iterator_has_next(&in_edges)) {
	    SCEDA_Edge *e = SCEDA_in_edges_iterator_next(&in_edges);
	    boxed(int) fe = SCEDA_hashmap_get(flow, e);
	    int rc = boxed_get(fe);
	    /** check whether e is in the residual graph */
	    if(rc <= 0) {
	      continue;
	    }
	    SCEDA_Vertex *v = SCEDA_edge_source(e);
	    boxed(double) piv = SCEDA_hashmap_get(pi, v);
	    double ce_red = -cost(e, cost_ctxt) - boxed_get(piu) + boxed_get(piv);
	    /** check whether e is an admissible edge */
	    if((-epsilon <= ce_red) && (ce_red < 0)) {
	      int push = boxed_get(eu);
	      if(push > rc) {
		push = rc;
	      }
	      boxed_set(fe, boxed_get(fe) - push);

	      boxed(int) ev = SCEDA_hashmap_get(excess, v);
	      boxed_set(eu, boxed_get(eu) - push);
	      boxed_set(ev, boxed_get(ev) + push);
	      if((boxed_get(ev) > 0) && (boxed_get(ev) <= push)) {
		safe_call(SCEDA_queue_enqueue(todo, v)); 
	      }
	      if(boxed_get(eu) == 0) {
		break;
	      }
	    } else {
	      if(ce_red >= 0) {
		if((ce_red_min < 0) || (ce_red < ce_red_min)) {
		  ce_red_min = ce_red;
		} 
	      }
	    }
	  }
	  SCEDA_in_edges_iterator_cleanup(&in_edges);
	}

	if(boxed_get(eu) > 0) {
	  if(ce_red_min < 0) {
	    ce_red_min = 0;
	  }
	  boxed_set(piu, boxed_get(piu) + ce_red_min + epsilon);
	  relabelling++;
	}
      } while(boxed_get(eu) > 0);
    }

    SCEDA_queue_delete(todo);
  }

  SCEDA_hashmap_delete(pi);
  SCEDA_hashmap_delete(excess);
}
#endif

#ifdef SCEDA_minimise_flow_cost_cost_scaling_wave
static void SCEDA_minimise_flow_cost_cost_scaling_wave(SCEDA_Graph *g, 
						       SCEDA_int_edge_fun cap, void *cap_ctxt, 
						       SCEDA_int_edge_fun cost, void *cost_ctxt, 
						       SCEDA_HashMap *flow) {
  int n = SCEDA_graph_vcount(g);

  SCEDA_HashMap *excess = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);

  SCEDA_HashMap *pi = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);

  SCEDA_DList *topsort = SCEDA_dlist_create(NULL);

  double epsilon = 1;

  /* Initialise excess, pi, and epsilon */
  {
    /** we have excess(v) = supply(v) + sum(e \in In(v)) f(e) - sum(e \in Out(v)) f(e) */
    /** since f is a flow, excess(v) = 0 for any v */
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *u = SCEDA_vertices_iterator_next(&vertices);
      safe_call(SCEDA_hashmap_put(excess, u, boxed_create(int, 0), NULL));
      safe_call(SCEDA_hashmap_put(pi, u, boxed_create(double, 0), NULL));
      safe_call(SCEDA_dlist_ins_next(topsort, NULL, u));
    }
    SCEDA_vertices_iterator_cleanup(&vertices);

    int C = 0;

    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
      int ce = cost(e, cost_ctxt);
      if(ABS(ce) > C) {
	C = ABS(ce);
      }
    }
    SCEDA_edges_iterator_cleanup(&edges);

    epsilon = 1;
    while(epsilon < C) {
      epsilon *= 2;
    }
  }

  while(n * epsilon >= 1) {
    {
      epsilon = epsilon / 2;

      SCEDA_EdgesIterator edges;
      SCEDA_edges_iterator_init(g, &edges);
      while(SCEDA_edges_iterator_has_next(&edges)) {
	SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
	int ce = cost(e, cost_ctxt);
	SCEDA_Vertex *u = SCEDA_edge_source(e);
	SCEDA_Vertex *v = SCEDA_edge_target(e);
	double ce_red = ce - boxed_get(((boxed(double))SCEDA_hashmap_get(pi, u))) + boxed_get(((boxed(double))SCEDA_hashmap_get(pi, v)));
	if(ce_red > 0) {
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  boxed(int) eu = SCEDA_hashmap_get(excess, u);
	  boxed(int) ev = SCEDA_hashmap_get(excess, v);
	  boxed_set(eu, boxed_get(eu) + boxed_get(fe));
	  boxed_set(ev, boxed_get(ev) - boxed_get(fe));
	  boxed_set(fe, 0);
	} else if(ce_red < 0) {
	  boxed(int) fe = SCEDA_hashmap_get(flow, e);
	  int ucap = cap(e, cap_ctxt);
	  boxed(int) eu = SCEDA_hashmap_get(excess, u);
	  boxed(int) ev = SCEDA_hashmap_get(excess, v);
	  boxed_set(eu, boxed_get(eu) + boxed_get(fe) - ucap);
	  boxed_set(ev, boxed_get(ev) - boxed_get(fe) + ucap);
	  boxed_set(fe, ucap);
	}
      }
      SCEDA_edges_iterator_cleanup(&edges);
    }

    int in_excess = 0;

    {
      SCEDA_VerticesIterator vertices;
      SCEDA_vertices_iterator_init(g, &vertices);
      while(SCEDA_vertices_iterator_has_next(&vertices)) {
	SCEDA_Vertex *u = SCEDA_vertices_iterator_next(&vertices);
	boxed(int) eu = SCEDA_hashmap_get(excess, u);
	if(boxed_get(eu) > 0) {
	  in_excess++;
	}
      }
      SCEDA_vertices_iterator_cleanup(&vertices);
    }

    SCEDA_DListElt *current = SCEDA_dlist_head(topsort);

    while(in_excess > 0) {
      safe_ptr(current);
      SCEDA_Vertex *u = SCEDA_dlist_data(SCEDA_Vertex *, current);

      boxed(int) eu = SCEDA_hashmap_get(excess, u);
      if(boxed_get(eu) <= 0) {
	current = SCEDA_dlist_next(current);
	continue;
      }

      boxed(double) piu = SCEDA_hashmap_get(pi, u);
      do {
	double ce_red_min = -1;

	{
	  SCEDA_OutEdgesIterator out_edges;
	  SCEDA_out_edges_iterator_init(u, &out_edges);
	  while(SCEDA_out_edges_iterator_has_next(&out_edges)) {
	    SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&out_edges);
	    boxed(int) fe = SCEDA_hashmap_get(flow, e);
	    int rc = cap(e, cap_ctxt) - boxed_get(fe);
	    /** check whether e is in the residual graph */
	    if(rc <= 0) {
	      continue;
	    }
	    SCEDA_Vertex *v = SCEDA_edge_target(e);
	    boxed(double) piv = SCEDA_hashmap_get(pi, v);
	    double ce_red = cost(e, cost_ctxt) - boxed_get(piu) + boxed_get(piv);
	    /** check whether e is an admissible edge */
	    if((-epsilon <= ce_red) && (ce_red < 0)) {
	      int push = boxed_get(eu);
	      if(push > rc) {
		push = rc;
	      }
	      boxed_set(fe, boxed_get(fe) + push);

	      boxed(int) ev = SCEDA_hashmap_get(excess, v);

	      boxed_set(eu, boxed_get(eu) - push);
	      boxed_set(ev, boxed_get(ev) + push);
	      if((boxed_get(ev) > 0) && (boxed_get(ev) <= push)) {
		in_excess++;
	      }

	      if(boxed_get(eu) == 0) {
		break;
	      }
	    } else {
	      if(ce_red >= 0) {
		if((ce_red_min < 0) || (ce_red < ce_red_min)) {
		  ce_red_min = ce_red;
		} 
	      }
	    }
	  }
	  SCEDA_out_edges_iterator_cleanup(&out_edges);
	}

	if(boxed_get(eu) > 0) {
	  SCEDA_InEdgesIterator in_edges;
	  SCEDA_in_edges_iterator_init(u, &in_edges);
	  while(SCEDA_in_edges_iterator_has_next(&in_edges)) {
	    SCEDA_Edge *e = SCEDA_in_edges_iterator_next(&in_edges);
	    boxed(int) fe = SCEDA_hashmap_get(flow, e);
	    int rc = boxed_get(fe);
	    /** check whether e is in the residual graph */
	    if(rc <= 0) {
	      continue;
	    }
	    SCEDA_Vertex *v = SCEDA_edge_source(e);
	    boxed(double) piv = SCEDA_hashmap_get(pi, v);
	    double ce_red = -cost(e, cost_ctxt) - boxed_get(piu) + boxed_get(piv);
	    /** check whether e is an admissible edge */
	    if((-epsilon <= ce_red) && (ce_red < 0)) {
	      int push = boxed_get(eu);
	      if(push > rc) {
		push = rc;
	      }
	      boxed_set(fe, boxed_get(fe) - push);

	      boxed(int) ev = SCEDA_hashmap_get(excess, v);

	      boxed_set(eu, boxed_get(eu) - push);
	      boxed_set(ev, boxed_get(ev) + push);
	      if((boxed_get(ev) > 0) && (boxed_get(ev) <= push)) {
		in_excess++;
	      }

	      if(boxed_get(eu) == 0) {
		break;
	      }
	    } else {
	      if(ce_red >= 0) {
		if((ce_red_min < 0) || (ce_red < ce_red_min)) {
		  ce_red_min = ce_red;
		} 
	      }
	    }
	  }
	  SCEDA_in_edges_iterator_cleanup(&in_edges);
	}

	if(boxed_get(eu) > 0) {
	  if(ce_red_min < 0) {
	    ce_red_min = 0;
	  }
	  boxed_set(piu, boxed_get(piu) + ce_red_min + epsilon);
	  
	  if(current != SCEDA_dlist_head(topsort)) {
	    safe_call(SCEDA_dlist_rem(topsort, current, (void **)&u));
	    safe_call(SCEDA_dlist_ins_next(topsort, NULL, u));
	    current = SCEDA_dlist_head(topsort);
	  }
	}
      } while(boxed_get(eu) > 0);

      in_excess--;
      current = SCEDA_dlist_next(current);
    }
  }

  SCEDA_dlist_delete(topsort);

  SCEDA_hashmap_delete(pi);
  SCEDA_hashmap_delete(excess);
}
#endif

SCEDA_HashMap *SCEDA_graph_min_cost_max_flow(SCEDA_Graph *g, SCEDA_Vertex *s, SCEDA_Vertex *t, 
					     SCEDA_int_edge_fun capacity, void *cap_ctxt, 
					     SCEDA_int_edge_fun cost, void *cost_ctxt) {
  SCEDA_HashMap *flow = SCEDA_graph_max_flow(g, s, t, capacity, cap_ctxt);

  if(flow != NULL) {
    SCEDA_minimise_flow_cost(g, capacity, cap_ctxt, cost, cost_ctxt, flow);
  }

  return flow;
}

typedef struct {
  SCEDA_Vertex *s;
  SCEDA_Vertex *t;
  SCEDA_int_edge_fun capacity;
  void *cap_ctxt;
  SCEDA_HashMap *ncap;
} FFCtxt;

static int SCEDA_FF_cap(SCEDA_Edge *e, FFCtxt *ctxt) {
  if((SCEDA_edge_source(e) == ctxt->s) || (SCEDA_edge_target(e) == ctxt->t)) {
    boxed(int) cap = SCEDA_hashmap_get(ctxt->ncap, e);
    return boxed_get(cap);
  } else {
    return ctxt->capacity(e, ctxt->cap_ctxt);
  }
}

/** Important: supply is called exactly once per vertex, before modification of its neighborhood */
SCEDA_HashMap *SCEDA_graph_feasible_flow(SCEDA_Graph *g,
					 SCEDA_int_edge_fun capacity, void *cap_ctxt,
					 SCEDA_int_vertex_fun supply, void *sup_ctxt) {
  SCEDA_Vertex *s = SCEDA_graph_add_vertex(g, NULL);
  SCEDA_Vertex *t = SCEDA_graph_add_vertex(g, NULL);
  SCEDA_List *new_edges = SCEDA_list_create(NULL);
  SCEDA_HashMap *ncap = SCEDA_edge_map_create((SCEDA_delete_fun)boxed_delete);

  {
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertices);
      if((v == s) || (v == t)) {
	continue;
      }
      int sup = supply(v, sup_ctxt);
      if(sup > 0) {
	SCEDA_Edge *e = SCEDA_graph_add_edge(g, s, v, NULL);
	safe_call(SCEDA_list_add(new_edges, e));
	safe_call(SCEDA_hashmap_put(ncap, e, boxed_create(int, sup), NULL));
      } else if(sup < 0) {
	SCEDA_Edge *e = SCEDA_graph_add_edge(g, v, t, NULL);
	safe_call(SCEDA_list_add(new_edges, e));
	safe_call(SCEDA_hashmap_put(ncap, e, boxed_create(int, -sup), NULL));
      }
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  FFCtxt ctxt;
  ctxt.s = s;
  ctxt.t = t;
  ctxt.capacity = capacity;
  ctxt.cap_ctxt = cap_ctxt;
  ctxt.ncap = ncap;

  SCEDA_HashMap *flow = SCEDA_graph_max_flow(g, s, t, (SCEDA_int_edge_fun)SCEDA_FF_cap, (void *)&ctxt);
  
  safe_ptr(flow);

  int feasible = TRUE;
  
  {
    SCEDA_ListIterator edges;
    SCEDA_list_iterator_init(new_edges, &edges);
    while(SCEDA_list_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_list_iterator_next(&edges);
      boxed(int) ce = SCEDA_hashmap_get(ncap, e);
      boxed(int) fe;
      safe_call(SCEDA_hashmap_remove(flow, (void **)&e, (void **)&fe));
      if(boxed_get(fe) != boxed_get(ce)) {
	feasible = FALSE;
      }
      boxed_delete(fe);
      void *data;
      safe_call(SCEDA_graph_remove_edge(g, e, &data));
    }
    SCEDA_list_iterator_cleanup(&edges);

    SCEDA_hashmap_delete(ncap);

    SCEDA_list_delete(new_edges);

    void *data;
    safe_call(SCEDA_graph_remove_vertex(g, s, &data));
    safe_call(SCEDA_graph_remove_vertex(g, t, &data));
  }
  
  if(!feasible) {
    SCEDA_hashmap_delete(flow);
    return NULL;
  } else {
    return flow;
  }
}

typedef struct {
  SCEDA_int_edge_fun lcap;
  void *lcap_ctxt;
  SCEDA_int_edge_fun ucap;
  void *ucap_ctxt;
  SCEDA_int_vertex_fun supply;
  void *sup_ctxt;
} MCFCtxt;

static int SCEDA_MCF_cap(SCEDA_Edge *e, MCFCtxt *ctxt) {
  return ctxt->ucap(e, ctxt->ucap_ctxt) - ctxt->lcap(e, ctxt->lcap_ctxt);
}

static int SCEDA_MCF_supply(SCEDA_Vertex *v, MCFCtxt *ctxt) {
  int sup = ctxt->supply(v, ctxt->sup_ctxt);

  SCEDA_OutEdgesIterator out_edges;
  SCEDA_out_edges_iterator_init(v, &out_edges);
  while(SCEDA_out_edges_iterator_has_next(&out_edges)) {
    SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&out_edges);
    sup -= ctxt->lcap(e, ctxt->lcap_ctxt);
  }
  SCEDA_out_edges_iterator_cleanup(&out_edges);
  
  SCEDA_InEdgesIterator in_edges;
  SCEDA_in_edges_iterator_init(v, &in_edges);
  while(SCEDA_in_edges_iterator_has_next(&in_edges)) {
    SCEDA_Edge *e = SCEDA_in_edges_iterator_next(&in_edges);
    sup += ctxt->lcap(e, ctxt->lcap_ctxt);
  }
  SCEDA_in_edges_iterator_cleanup(&in_edges);
  
  return sup;
}

static int SCEDA_MCF_constant_cap(SCEDA_Edge *e, int *bound) {
  return *bound;
}

static int SCEDA_MCF_map_cost(SCEDA_Edge *e, SCEDA_HashMap *map) {
  int *n = SCEDA_hashmap_get(map, e);
  return *n;
}

static int SCEDA_MCF_map_supply(SCEDA_Vertex *v, SCEDA_HashMap *map) {
  int *n = SCEDA_hashmap_get(map, v);
  return *n;
}

SCEDA_HashMap *SCEDA_graph_min_cost_flow(SCEDA_Graph *g,
					 SCEDA_int_edge_fun lcap, void *lcap_ctxt,
					 SCEDA_int_edge_fun ucap, void *ucap_ctxt,
					 SCEDA_int_vertex_fun sup, void *sup_ctxt,
					 SCEDA_int_edge_fun cost, void *cost_ctxt) {
  MCFCtxt ctxt;
  if(sup == NULL) {
    sup = (SCEDA_int_vertex_fun)SCEDA_MCF_map_supply;
  }

  if(cost == NULL) {
    cost = (SCEDA_int_edge_fun)SCEDA_MCF_map_cost;
  }

  ctxt.supply = sup;
  ctxt.sup_ctxt = sup_ctxt;

  if(lcap == NULL) {
    if(lcap_ctxt != NULL) {
      lcap = (SCEDA_int_edge_fun)SCEDA_MCF_constant_cap;
    }
  }

  ctxt.lcap = lcap;
  ctxt.lcap_ctxt = lcap_ctxt;

  int B = 0;
  if(ucap == NULL) {
    ucap = (SCEDA_int_edge_fun)SCEDA_MCF_constant_cap;
    if(ucap_ctxt == NULL) {
      SCEDA_VerticesIterator vertices;
      SCEDA_vertices_iterator_init(g, &vertices);
      while(SCEDA_vertices_iterator_has_next(&vertices)) {
	SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertices);
	B += ABS(sup(v, sup_ctxt));
      }
      SCEDA_vertices_iterator_cleanup(&vertices);

      if(lcap != NULL) {
	SCEDA_EdgesIterator edges;
	SCEDA_edges_iterator_init(g, &edges);
	while(SCEDA_edges_iterator_has_next(&edges)) {
	  SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
	  B += ABS(lcap(e, lcap_ctxt));
	}
	SCEDA_edges_iterator_cleanup(&edges);
      }
      ucap_ctxt = &B;
    }
  }

  ctxt.ucap = ucap;
  ctxt.ucap_ctxt = ucap_ctxt;

  SCEDA_int_vertex_fun supply = sup;
  void *supply_ctxt = sup_ctxt;

  SCEDA_int_edge_fun cap = ucap;
  void *cap_ctxt = ucap_ctxt;

  if(lcap != NULL) {
    supply = (SCEDA_int_vertex_fun)SCEDA_MCF_supply;
    supply_ctxt = &ctxt; // use only lcap and supply (related) fields

    cap = (SCEDA_int_edge_fun)SCEDA_MCF_cap;
    cap_ctxt = &ctxt; // use only lcap and ucap (related) fields
  } 

  SCEDA_HashMap *flow = SCEDA_graph_feasible_flow(g, cap, cap_ctxt, supply, supply_ctxt);

  if(flow != NULL) {
    SCEDA_minimise_flow_cost(g, cap, cap_ctxt, cost, cost_ctxt, flow);

    if(lcap != NULL) {
      SCEDA_HashMapIterator phi;
      SCEDA_hashmap_iterator_init(flow, &phi);
      while(SCEDA_hashmap_iterator_has_next(&phi)) {
	void *e;
	boxed(int) fe = SCEDA_hashmap_iterator_next(&phi, &e);
	boxed_set(fe, boxed_get(fe) + lcap((SCEDA_Edge *)e, lcap_ctxt));
      }
      SCEDA_hashmap_iterator_cleanup(&phi);
    }
  }

  return flow;
}

