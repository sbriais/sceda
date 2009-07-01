/*
   Copyright 2008, 2009 Sebastien Briais
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
#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "graph_dag.h"

#include "util.h"

#include "queue.h"
#include "pqueue.h"

int SCEDA_graph_is_acyclic(SCEDA_Graph *g) {
  int n = SCEDA_graph_vcount(g);

  SCEDA_Vertex *g_vertice[n];
  int in_deg[n];
  int idx[n];

  int i = 0;

  int count = 0;
  SCEDA_Queue *q = SCEDA_queue_create(NULL);

  SCEDA_VerticesIterator vertice;
  SCEDA_vertices_iterator_init(g, &vertice);
  while(SCEDA_vertices_iterator_has_next(&vertice)) {
    SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&vertice);
    SCEDA_vertex_set_index(v, i);
    g_vertice[i] = v;
    in_deg[i] = SCEDA_vertex_in_deg(v);
    if(in_deg[i] == 0) {
      safe_call(SCEDA_queue_enqueue(q, v));
      idx[i] = count++;
    }
    i++;
  }
  SCEDA_vertices_iterator_cleanup(&vertice);

  while(!SCEDA_queue_is_empty(q)) {
    SCEDA_Vertex *v;
    safe_call(SCEDA_queue_dequeue(q, (void **)&v));

    SCEDA_VertexSuccIterator succ;
    SCEDA_vertex_succ_iterator_init(v, &succ);
    while(SCEDA_vertex_succ_iterator_has_next(&succ)) {
      SCEDA_Vertex *w = SCEDA_vertex_succ_iterator_next(&succ);
      int j = SCEDA_vertex_get_index(w);
      in_deg[j]--;
      if(in_deg[j] == 0) {
	safe_call(SCEDA_queue_enqueue(q,w));
	idx[j] = count++;
      }
    }
    SCEDA_vertex_succ_iterator_cleanup(&succ);
  }
  
  SCEDA_queue_delete(q);

  if(count == n) {
    for(i = 0; i < n; i++) {
      SCEDA_vertex_set_index(g_vertice[i], idx[i]);
    }
    return TRUE;
  } else {
    return FALSE;
  }
}

int SCEDA_graph_compute_topological_order(SCEDA_Graph *g) {
  if(SCEDA_graph_is_acyclic(g)) {
    return 0;
  }
  return -1;
}

static int compare_vertex(SCEDA_Vertex *v1, SCEDA_Vertex *v2) {
  int i = SCEDA_vertex_get_index(v1);
  int j = SCEDA_vertex_get_index(v2);
  if(i < j) {
    return -1;
  } else if(i == j) {
    return 0;
  } else {
    return 1;
  }
}

/** Goralcikova-Koubek algorithm for DAGs */
SCEDA_Graph *SCEDA_graph_transitive_closure(SCEDA_Graph *g) {
  safe_call(SCEDA_graph_compute_topological_order(g));

  int i;
  int n = SCEDA_graph_vcount(g);
  SCEDA_Vertex *vertice_f[n];
  int mark[n];

  // contains shallow copies of vertice of G
  SCEDA_Graph *gf = SCEDA_graph_create(NULL, NULL);

  SCEDA_VerticesIterator g_vertice;
  SCEDA_vertices_iterator_init(g, &g_vertice);
  while(SCEDA_vertices_iterator_has_next(&g_vertice)) {
    SCEDA_Vertex *vi = SCEDA_vertices_iterator_next(&g_vertice);
    i = SCEDA_vertex_get_index(vi);
    mark[i] = FALSE;
    SCEDA_Vertex *vi_f = SCEDA_graph_add_vertex(gf, vi); //create a corresponding vertex in Gf
    vertice_f[i] = vi_f;
    SCEDA_vertex_set_index(vi_f, i);
  }
  SCEDA_vertices_iterator_cleanup(&g_vertice);

  SCEDA_PQueue *h = SCEDA_pqueue_create(NULL, (SCEDA_compare_fun)compare_vertex);

  for(i = n-1; i >= 0; i--) {
    SCEDA_Vertex *vi_f = vertice_f[i];
    SCEDA_Vertex *vi = SCEDA_vertex_get_data(SCEDA_Vertex *, vi_f);
    
    SCEDA_VertexSuccIterator vi_succ;
    SCEDA_vertex_succ_iterator_init(vi, &vi_succ);
    while(SCEDA_vertex_succ_iterator_has_next(&vi_succ)) {
      SCEDA_Vertex *vj = SCEDA_vertex_succ_iterator_next(&vi_succ);
      int j = SCEDA_vertex_get_index(vj);
      SCEDA_Vertex *vj_f = vertice_f[j];
      safe_call(SCEDA_pqueue_insert(h, vj_f));
    }
    SCEDA_vertex_succ_iterator_cleanup(&vi_succ);

    while(SCEDA_pqueue_size(h) > 0) {
      SCEDA_Vertex *vj_f;
      safe_call(SCEDA_pqueue_extract(h, (void **)&vj_f));
      int j = SCEDA_vertex_get_index(vj_f);
      if(!mark[j]) {
	SCEDA_graph_add_edge(gf, vi_f, vj_f, NULL);

	SCEDA_VertexSuccIterator vj_succ;
	SCEDA_vertex_succ_iterator_init(vj_f, &vj_succ);
	while(SCEDA_vertex_succ_iterator_has_next(&vj_succ)) {
	  SCEDA_Vertex *w = SCEDA_vertex_succ_iterator_next(&vj_succ);
	  int k = SCEDA_vertex_get_index(w);
	  if(!mark[k]) {
	    mark[k] = TRUE;
	    SCEDA_graph_add_edge(gf, vi_f, w, NULL);
	  }
	}
	SCEDA_vertex_succ_iterator_cleanup(&vj_succ);
      }
    }

    SCEDA_vertex_succ_iterator_init(vi_f, &vi_succ);
    while(SCEDA_vertex_succ_iterator_has_next(&vi_succ)) {
      SCEDA_Vertex *v = SCEDA_vertex_succ_iterator_next(&vi_succ);
      mark[SCEDA_vertex_get_index(v)] = FALSE;      
    }
    SCEDA_vertex_succ_iterator_cleanup(&vi_succ);
  }

  SCEDA_pqueue_delete(h);

  return gf;
}
