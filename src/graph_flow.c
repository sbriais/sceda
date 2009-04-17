/*
   This file is part of SCEDA.
   (c) 2008-2009 Sebastien Briais
   
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

SCEDA_HashMap *SCEDA_graph_max_flow(SCEDA_Graph *g, SCEDA_Vertex *s, SCEDA_Vertex *t,
				    SCEDA_int_edge_fun capacity, void *c_ctxt) {
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
	SCEDA_hashmap_put(height, v, boxed_create(int, SCEDA_graph_vcount(g)), NULL);
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

  SCEDA_Queue *todo = SCEDA_queue_create(NULL);
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

  /* Preflow */
  while(!SCEDA_queue_is_empty(todo)) {
    SCEDA_Vertex *u;
    safe_call(SCEDA_queue_dequeue(todo, (void **)&u));
    if(u == t) {
      continue;
    }

    boxed(int) ex_u = SCEDA_hashmap_get(excess, u);
    boxed(int) h_u = SCEDA_hashmap_get(height, u);

    // push as much flow as possible
    {
      SCEDA_OutEdgesIterator edges;
      SCEDA_out_edges_iterator_init(u, &edges);
      while(SCEDA_out_edges_iterator_has_next(&edges)) {
	SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&edges);
	boxed(int) fe = SCEDA_hashmap_get(flow, e);
	// residual_capacity(u,v) = capacity(u,v) - flow(u,v) 
	int rc = capacity(e, c_ctxt) - boxed_get(fe);
	if(rc <= 0) {
	  continue;
	}
	SCEDA_Vertex *v = SCEDA_edge_target(e);
	boxed(int) h_v = SCEDA_hashmap_get(height, v);
	if(boxed_get(h_v) < boxed_get(h_u)) {
	  // h(u) = h(v) + 1 so we can push
	  boxed(int) ex_v = SCEDA_hashmap_get(excess, v);
	  if(boxed_get(ex_v) == 0) {
	    // v was not exceeding (hence not in the queue)
	    // but will be exceeding now
	    safe_call(SCEDA_queue_enqueue(todo, v));
	  }
	  // amount of flow to push 
	  int push = boxed_get(ex_u);
	  if(push > rc) {
	    // cannot push more than the residual capacity
	    push = rc;
	  }
	  // update excess values 
	  boxed_set(fe, boxed_get(fe) + push);
	  boxed_set(ex_v, boxed_get(ex_v) + push);
	  boxed_set(ex_u, boxed_get(ex_u) - push);
	  if(boxed_get(ex_u) == 0) {
	    // nothing to push anymore
	    break;
	  }
	}
      }
      SCEDA_out_edges_iterator_cleanup(&edges);
    }

    // push back as much flow as possible
    if(boxed_get(ex_u) > 0) {
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
	  boxed(int) ex_v = SCEDA_hashmap_get(excess, v);
	  if(boxed_get(ex_v) == 0) {
	    safe_call(SCEDA_queue_enqueue(todo, v));
	  }
	  int push = boxed_get(ex_u);
	  if(push > boxed_get(fe)) {
	    // cannot push back more than the flow
	    push = boxed_get(fe);
	  }
	  boxed_set(fe, boxed_get(fe) - push);
	  boxed_set(ex_v, boxed_get(ex_v) + push);
	  boxed_set(ex_u, boxed_get(ex_u) - push);
	  if(boxed_get(ex_u) == 0) {
	    // nothing to push anymore
	    break;
	  }
	}
      }
      SCEDA_in_edges_iterator_cleanup(&edges);
    }

    // if u is still exceeding, increase its height
    if(boxed_get(ex_u) > 0) {
      boxed_set(h_u, boxed_get(h_u) + 1);
      safe_call(SCEDA_queue_enqueue(todo, u));
    }
  }

  /* Finished */

  SCEDA_queue_delete(todo);

  SCEDA_hashmap_delete(height);
  SCEDA_hashmap_delete(excess);

  return flow;
}

static int SCEDA_augment_flow_along_neg_cycle(SCEDA_Graph *g, 
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

SCEDA_HashMap *SCEDA_graph_min_cost_max_flow(SCEDA_Graph *g, SCEDA_Vertex *s, SCEDA_Vertex *t, 
					     SCEDA_int_edge_fun capacity, void *cap_ctxt, 
					     SCEDA_int_edge_fun cost, void *cost_ctxt) {
  if(s == t) {
    return NULL;
  }

  SCEDA_HashMap *flow = SCEDA_graph_max_flow(g, s, t, capacity, cap_ctxt);

  while(SCEDA_augment_flow_along_neg_cycle(g, capacity, cap_ctxt, cost, cost_ctxt, flow)) {
  }

  return flow;
}
