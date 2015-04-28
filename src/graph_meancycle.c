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

#include "graph_dag.h"
#include "graph_meancycle.h"
#include "boxed.h"

int SCEDA_graph_minimum_mean_cycle(SCEDA_Graph *g,
				   SCEDA_int_edge_fun cost, void *c_ctxt,
				   SCEDA_List **min_cycle) {
  int n = SCEDA_graph_vcount(g);

  SCEDA_HashMap *dist[n+1];
  SCEDA_HashMap *incoming_edge[n+1];
  {
    int i;
    for(i = 0; i < n+1; i++) {
      dist[i] = SCEDA_vertex_map_create((SCEDA_delete_fun)boxed_delete);
      incoming_edge[i] = SCEDA_vertex_map_create(NULL);
    }
  }

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

	SCEDA_InEdgesIterator in_edges;
	SCEDA_in_edges_iterator_init(v, &in_edges);
	while(SCEDA_in_edges_iterator_has_next(&in_edges)) {
	  SCEDA_Edge *e = SCEDA_in_edges_iterator_next(&in_edges);

	  SCEDA_Vertex *u = SCEDA_edge_source(e);

	  boxed(int) du = SCEDA_hashmap_get(dist[i-1],u);
	  if(du == NULL) {
	    continue;
	  }

	  int ce = cost(e, c_ctxt);

	  if((in_v == NULL) || (boxed_get(du) + ce < boxed_get(dv))) {
	    in_v = e;
	    boxed_set(dv, boxed_get(du) + ce);
	  }
	}
	SCEDA_in_edges_iterator_cleanup(&in_edges);

	if(in_v != NULL) {
	  SCEDA_hashmap_put(dist[i], v, dv, NULL);
	  SCEDA_hashmap_put(incoming_edge[i], v, in_v, NULL);
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

  if(mu_v != NULL ) {
    SCEDA_HashSet *in_cycle = SCEDA_vertex_set_create();
    SCEDA_Vertex *cycle = mu_v;
    int k = n;
    do {
      SCEDA_hashset_add(in_cycle, cycle);
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edge[k], cycle);
      safe_ptr(e);
      cycle = SCEDA_edge_source(e);
      k--;
    } while(!SCEDA_hashset_contains(in_cycle, cycle));
    SCEDA_hashset_delete(in_cycle);

    SCEDA_Vertex *v = mu_v;
    k = n;
    while(v != cycle) {
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edge[k], v);
      v = SCEDA_edge_source(e);
      k--;
    }

    *min_cycle = SCEDA_list_create(NULL);
    do {
      SCEDA_Edge *e = SCEDA_hashmap_get(incoming_edge[k], v);
      SCEDA_list_add(*min_cycle, e);
      v = SCEDA_edge_source(e);
      k--;
    } while(v != cycle);
  }

  {
    int i;
    for(i = 0; i < n+1; i++) {
      SCEDA_hashmap_delete(dist[i]);
      SCEDA_hashmap_delete(incoming_edge[i]);
    }
  }

  if(mu_v != NULL) {
    return 0;
  } else {
    return -1;
  }
}
