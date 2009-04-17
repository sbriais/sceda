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
#include "graph_dag.h"
#include "graph_matching.h"
#include "graph_antichain.h"
#include "util.h"
#include "common.h"
#include "list.h"
#include "hashmap.h"

// the resulting graph will contain pointers on vertice of G
static SCEDA_Graph *SCEDA_graph_split(const SCEDA_Graph *g, SCEDA_List **y_vert, SCEDA_List **z_vert) {
  // the split of a graph G = (X,U) is a bipartite graph BG = (Y \cup Z, E)
  // where Y and Z are two distinct copies of X 
  // and (y_i,z_j) \in E \iff (x_i,x_j) \in U
  // where y_i is the copy of x_i in Y
  //   and z_j is the copy of x_j in Z

  SCEDA_Graph *bg = SCEDA_graph_create(NULL,NULL);

  int n = SCEDA_graph_vcount(g);

  SCEDA_Vertex *v_y[n];
  SCEDA_Vertex *v_z[n];

  SCEDA_VerticesIterator g_vertice;
  SCEDA_vertices_iterator_init(g, &g_vertice);
  while(SCEDA_vertices_iterator_has_next(&g_vertice)) {
    SCEDA_Vertex *vx = SCEDA_vertices_iterator_next(&g_vertice);
    int i = SCEDA_vertex_get_index(vx);
    SCEDA_Vertex *vy = SCEDA_graph_add_vertex(bg, vx);
    SCEDA_Vertex *vz = SCEDA_graph_add_vertex(bg, vx);
    v_y[i] = vy;
    v_z[i] = vz;
    SCEDA_vertex_set_index(vy, 2*i);
    SCEDA_vertex_set_index(vz, 2*i+1);
  }
  SCEDA_vertices_iterator_cleanup(&g_vertice);

  SCEDA_vertices_iterator_init(g, &g_vertice);
  while(SCEDA_vertices_iterator_has_next(&g_vertice)) {
    SCEDA_Vertex *vi = SCEDA_vertices_iterator_next(&g_vertice);
    int i = SCEDA_vertex_get_index(vi);
    SCEDA_Vertex *vy = v_y[i];
    SCEDA_VertexSuccIterator vi_succ;
    SCEDA_vertex_succ_iterator_init(vi, &vi_succ);
    while(SCEDA_vertex_succ_iterator_has_next(&vi_succ)) {
      SCEDA_Vertex *vj = SCEDA_vertex_succ_iterator_next(&vi_succ);
      int j = SCEDA_vertex_get_index(vj);
      SCEDA_Vertex *vz = v_z[j];
      SCEDA_graph_add_edge(bg, vy, vz, NULL);
    }
    SCEDA_vertex_succ_iterator_cleanup(&vi_succ);
  }
  SCEDA_vertices_iterator_cleanup(&g_vertice);

  *y_vert = SCEDA_list_create(NULL);
  *z_vert = SCEDA_list_create(NULL);

  int i;
  for(i = n-1; i >= 0; i--) {
    safe_call(SCEDA_list_ins_next(*y_vert, NULL, v_y[i]));
    safe_call(SCEDA_list_ins_next(*z_vert, NULL, v_z[i]));	      
  }

  return bg;
}

// compute minimal chain cover of G
// set next_in_chain and prev_in_chain in G
// return the number of chains
int SCEDA_graph_minimum_chain_cover(SCEDA_Graph *gf, SCEDA_HashMap **prev_in_chain, SCEDA_HashMap **next_in_chain) {
#define SCEDA_vertex_set_prev_in_chain(v, prev) (SCEDA_hashmap_put(*prev_in_chain, v, prev, NULL))
#define SCEDA_vertex_set_next_in_chain(v, next) (SCEDA_hashmap_put(*next_in_chain, v, next, NULL))

  *prev_in_chain = SCEDA_vertex_map_create(NULL);
  *next_in_chain = SCEDA_vertex_map_create(NULL);

  SCEDA_List *y_vert;
  SCEDA_List *z_vert;

  // compute the split of Gf
  SCEDA_Graph *bg = SCEDA_graph_split(gf, &y_vert, &z_vert); // based upon Gf vertice
  
  SCEDA_HashMap *spouse = SCEDA_graph_maximum_bipartite_matching(bg, y_vert, z_vert);

#define SCEDA_vertex_get_spouse(v) (SCEDA_hashmap_get(spouse, v))

  int n = SCEDA_graph_vcount(gf);

  SCEDA_Vertex *v_y[n];

  SCEDA_ListIterator vertice;
  SCEDA_list_iterator_init(y_vert, &vertice);
  while(SCEDA_list_iterator_has_next(&vertice)) {
    SCEDA_Vertex *vy = SCEDA_list_iterator_next(&vertice);
    int i = SCEDA_vertex_get_index(vy) / 2;
    v_y[i] = vy;
  }
  SCEDA_list_iterator_cleanup(&vertice);

  int count = 0;
  SCEDA_list_iterator_init(z_vert, &vertice);
  while(SCEDA_list_iterator_has_next(&vertice)) {
    SCEDA_Vertex *vz = SCEDA_list_iterator_next(&vertice);
    SCEDA_Vertex *vy = SCEDA_vertex_get_spouse(vz);
    if(vy == NULL) {
      count++;
      SCEDA_Vertex *v = SCEDA_vertex_get_data(SCEDA_Vertex *, vz);
      SCEDA_vertex_set_prev_in_chain(v, NULL);

      for(;;) {
	int i = SCEDA_vertex_get_index(v);
	vy = v_y[i];
	vz = SCEDA_vertex_get_spouse(vy);
	if(vz == NULL) {
	  SCEDA_vertex_set_next_in_chain(v, NULL);
	  break;
	} else {
	  SCEDA_Vertex *w = SCEDA_vertex_get_data(SCEDA_Vertex *, vz);
	  SCEDA_vertex_set_prev_in_chain(w, v);
	  SCEDA_vertex_set_next_in_chain(v, w);
	  v = w;
	}
      }
    }
  }
  SCEDA_list_iterator_cleanup(&vertice);

  SCEDA_hashmap_delete(spouse);

#undef SCEDA_vertex_get_spouse

  // this is not needed anymore
  SCEDA_list_delete(y_vert);
  SCEDA_list_delete(z_vert);
  SCEDA_graph_delete(bg);

  return count;

#undef SCEDA_vertex_set_prev_in_chain
#undef SCEDA_vertex_set_next_in_chain
}

static SCEDA_List *SCEDA_graph_maximum_antichain_aux(SCEDA_Graph *gf) {
  SCEDA_HashMap *prev_in_chain;
  SCEDA_HashMap *next_in_chain;

#define SCEDA_vertex_get_prev_in_chain(v) (SCEDA_hashmap_get(prev_in_chain, v))
#define SCEDA_vertex_get_next_in_chain(v) (SCEDA_hashmap_get(next_in_chain, v))

  int count = SCEDA_graph_minimum_chain_cover(gf, &prev_in_chain, &next_in_chain);

  // initialise antichain with max elements
  SCEDA_Vertex *antichain[count];

  int i = 0;
  SCEDA_VerticesIterator vertice;
  SCEDA_vertices_iterator_init(gf, &vertice);
  while(SCEDA_vertices_iterator_has_next(&vertice)) {
    SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertice);
    if(SCEDA_vertex_get_next_in_chain(v) == NULL) {
      antichain[i++] = v;
    }
  }
  SCEDA_vertices_iterator_cleanup(&vertice);

  // assert(i == count);

  // walk backward in the chains until finding an antichain
  int is_antichain = FALSE;
  while(!is_antichain) {
    is_antichain = TRUE;
    for(i = 0; i < count; i++) {
      SCEDA_Vertex *vi = antichain[i];
      int j;
      for(j = 0; j < count; j++) {
	if(j == i) {
	  continue;
	}
	SCEDA_Vertex *vj = antichain[j];
	if(SCEDA_vertex_is_succ_of(vi, vj)) {
	  is_antichain = FALSE;
	  antichain[i] = SCEDA_vertex_get_prev_in_chain(vi); // walk backward in the chain
	  // assert(antichain[i] != NULL);
	  break;
	}
      }
    }
  }

  SCEDA_List *result = SCEDA_list_create(NULL);
  for(i = 0; i < count; i++) {
    SCEDA_Vertex *vi = antichain[i];
    safe_call(SCEDA_list_ins_next(result, NULL, SCEDA_vertex_get_data(SCEDA_Vertex *, vi)));
  }

  SCEDA_hashmap_delete(prev_in_chain);
  SCEDA_hashmap_delete(next_in_chain);

#undef SCEDA_vertex_get_prev_in_chain
#undef SCEDA_vertex_get_next_in_chain

  return result;
}

SCEDA_List *SCEDA_graph_maximum_antichain(SCEDA_Graph *g) { 
  // first compute the transitive closure Gf of G
  safe_call(SCEDA_graph_compute_topological_order(g));
  SCEDA_Graph *gf = SCEDA_graph_transitive_closure(g);
  SCEDA_List *result = SCEDA_graph_maximum_antichain_aux(gf);
  SCEDA_graph_delete(gf);
  return result;
}
