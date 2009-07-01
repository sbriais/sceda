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
#include "graph_scc.h"

#include "hashset.h"
#include "stack.h"

#include "util.h"
#include "common.h"

static void SCEDA_DFT1(SCEDA_Vertex *u, SCEDA_HashSet *visited, SCEDA_Stack *ordered_vertices) {
  safe_call(SCEDA_hashset_add(visited, u));

  SCEDA_VertexSuccIterator succ;
  SCEDA_vertex_succ_iterator_init(u, &succ);
  while(SCEDA_vertex_succ_iterator_has_next(&succ)) {
    SCEDA_Vertex *v = SCEDA_vertex_succ_iterator_next(&succ);
    if(!SCEDA_hashset_contains(visited, v)) {
      SCEDA_DFT1(v, visited, ordered_vertices);
    }
  }
  SCEDA_vertex_succ_iterator_cleanup(&succ);

  safe_call(SCEDA_stack_push(ordered_vertices, u));
}

static void SCEDA_DFT2(SCEDA_Vertex *u, SCEDA_HashSet *visited, SCEDA_HashSet *cc) {
  safe_call(SCEDA_hashset_add(visited, u));

  SCEDA_VertexPredIterator pred;
  SCEDA_vertex_pred_iterator_init(u, &pred);
  while(SCEDA_vertex_pred_iterator_has_next(&pred)) {
    SCEDA_Vertex *v = SCEDA_vertex_pred_iterator_next(&pred);
    if(!SCEDA_hashset_contains(visited, v)) {
      SCEDA_DFT2(v, visited, cc);
    }
  }
  SCEDA_vertex_pred_iterator_cleanup(&pred);

  safe_call(SCEDA_hashset_add(cc, u));
}

SCEDA_List *SCEDA_graph_strongly_connected_components(SCEDA_Graph *g) {
  SCEDA_HashSet *visited = SCEDA_vertex_set_create();

  SCEDA_Stack *ordered_vertices = SCEDA_stack_create(NULL);

  {
    SCEDA_VerticesIterator vertices;
    SCEDA_vertices_iterator_init(g, &vertices);
    while(SCEDA_vertices_iterator_has_next(&vertices)) {
      SCEDA_Vertex *u = SCEDA_vertices_iterator_next(&vertices);
      if(!SCEDA_hashset_contains(visited, u)) {
	SCEDA_DFT1(u, visited, ordered_vertices);
      }
    }
    SCEDA_vertices_iterator_cleanup(&vertices);
  }

  SCEDA_List *scc = SCEDA_list_create((SCEDA_delete_fun)SCEDA_hashset_delete);

  SCEDA_hashset_clear(visited);
  
  {
    while(!SCEDA_stack_is_empty(ordered_vertices)) {
      SCEDA_Vertex *u;
      safe_call(SCEDA_stack_pop(ordered_vertices, (void **)&u));
      if(!SCEDA_hashset_contains(visited, u)) {
	SCEDA_HashSet *cc = SCEDA_vertex_set_create();
	safe_call(SCEDA_list_add(scc, cc));
	SCEDA_DFT2(u, visited, cc);
      }
    }
  }

  SCEDA_stack_delete(ordered_vertices);

  SCEDA_hashset_delete(visited);

  return scc;
}

