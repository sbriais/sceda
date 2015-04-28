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
#include "graph_traversal.h"
#include "util.h"

/** Breadth First Search */
void SCEDA_bfs_iterator_init(SCEDA_Vertex *v, SCEDA_BFSIterator *iter) {
  iter->to_visit = SCEDA_queue_create(NULL);
  iter->visited = SCEDA_vertex_set_create();
  safe_call(SCEDA_queue_enqueue(iter->to_visit, v));
  safe_call(SCEDA_hashset_add(iter->visited, v));
}

int SCEDA_bfs_iterator_has_next(SCEDA_BFSIterator *iter) {
  return (!SCEDA_queue_is_empty(iter->to_visit));
}

SCEDA_Vertex *SCEDA_bfs_iterator_next(SCEDA_BFSIterator *iter) {
  SCEDA_Vertex *v;
  safe_call(SCEDA_queue_dequeue(iter->to_visit, (void **)&v));
  SCEDA_VertexSuccIterator succ;
  SCEDA_vertex_succ_iterator_init(v, &succ);
  while(SCEDA_vertex_succ_iterator_has_next(&succ)) {
    SCEDA_Vertex *w = SCEDA_vertex_succ_iterator_next(&succ);
    if(!SCEDA_hashset_contains(iter->visited, w)) {
      safe_call(SCEDA_queue_enqueue(iter->to_visit, w));
      safe_call(SCEDA_hashset_add(iter->visited, w));
    }
  }
  SCEDA_vertex_succ_iterator_cleanup(&succ);
  return v;
}

void SCEDA_bfs_iterator_cleanup(SCEDA_BFSIterator *iter) {
  SCEDA_queue_delete(iter->to_visit);
  SCEDA_hashset_delete(iter->visited);
  memset(iter, 0, sizeof(SCEDA_BFSIterator));
}

/** Depth First Search */
void SCEDA_dfs_iterator_init(SCEDA_Vertex *v, SCEDA_DFSIterator *iter) {
  iter->to_visit = SCEDA_stack_create(NULL);
  iter->visited = SCEDA_vertex_set_create();
  safe_call(SCEDA_stack_push(iter->to_visit, v));
  safe_call(SCEDA_hashset_add(iter->visited, v));
}

int SCEDA_dfs_iterator_has_next(SCEDA_DFSIterator *iter) {
  return (!SCEDA_stack_is_empty(iter->to_visit));
}

SCEDA_Vertex *SCEDA_dfs_iterator_next(SCEDA_DFSIterator *iter) {
  SCEDA_Vertex *v;
  safe_call(SCEDA_stack_pop(iter->to_visit, (void **)&v));
  SCEDA_VertexSuccIterator succ;
  SCEDA_vertex_succ_iterator_init(v, &succ);
  while(SCEDA_vertex_succ_iterator_has_next(&succ)) {
    SCEDA_Vertex *w = SCEDA_vertex_succ_iterator_next(&succ);
    if(!SCEDA_hashset_contains(iter->visited, w)) {
      safe_call(SCEDA_stack_push(iter->to_visit, w));
      safe_call(SCEDA_hashset_add(iter->visited, w));
    }
  }
  SCEDA_vertex_succ_iterator_cleanup(&succ);
  return v;
}

void SCEDA_dfs_iterator_cleanup(SCEDA_DFSIterator *iter) {
  SCEDA_stack_delete(iter->to_visit);
  SCEDA_hashset_delete(iter->visited);
  memset(iter, 0, sizeof(SCEDA_DFSIterator));
}
