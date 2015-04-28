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
#ifndef __SCEDA_GRAPH_TRAVERSAL_H
#define __SCEDA_GRAPH_TRAVERSAL_H
/** \file graph_traversal.h
    \brief several graph traversal iterators */

#include "graph.h"
#include "hashset.h"
#include "stack.h"
#include "queue.h"

typedef struct {
  SCEDA_HashSet *visited;
  SCEDA_Stack *to_visit;
} SCEDA_DFSIterator;

typedef struct {
  SCEDA_HashSet *visited;
  SCEDA_Queue *to_visit;
} SCEDA_BFSIterator;

void SCEDA_bfs_iterator_init(SCEDA_Vertex *v, SCEDA_BFSIterator *iter);
int SCEDA_bfs_iterator_has_next(SCEDA_BFSIterator *iter);
SCEDA_Vertex *SCEDA_bfs_iterator_next(SCEDA_BFSIterator *iter);
void SCEDA_bfs_iterator_cleanup(SCEDA_BFSIterator *iter);

void SCEDA_dfs_iterator_init(SCEDA_Vertex *v, SCEDA_DFSIterator *iter);
int SCEDA_dfs_iterator_has_next(SCEDA_DFSIterator *iter);
SCEDA_Vertex *SCEDA_dfs_iterator_next(SCEDA_DFSIterator *iter);
void SCEDA_dfs_iterator_cleanup(SCEDA_DFSIterator *iter);

#endif
