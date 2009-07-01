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
#include <string.h>
#include "hashset.h"
#include "graph.h"
#include "util.h"

#define SCEDA_vertex_id(v) ((v)->id)

static inline void SCEDA_vertex_delete_struct(SCEDA_Vertex *v) {
  SCEDA_hashmap_delete(v->in_edges);
  SCEDA_hashmap_delete(v->out_edges);
  free(v);
}

static void SCEDA_vertex_delete(SCEDA_Vertex *v) {
  SCEDA_delete_fun delete = v->delete;
  if(delete != NULL) {
    delete(v->data);
  }
  SCEDA_vertex_delete_struct(v);
}

static int SCEDA_vertex_match(SCEDA_Vertex *v1, SCEDA_Vertex *v2) {
  return v1 == v2;
}

static int SCEDA_vertex_hash(SCEDA_Vertex *v) {
  return v->id;    
}

static inline void SCEDA_edge_delete_struct(SCEDA_Edge *e) {
  free(e);
}

static void SCEDA_edge_delete(SCEDA_Edge *e) {
  SCEDA_delete_fun delete = e->delete;
  if(delete != NULL) {
    delete(e->data);
  }
  SCEDA_edge_delete_struct(e);
}

static int SCEDA_edge_match(SCEDA_Edge *e1, SCEDA_Edge *e2) {
  return e1 == e2;
}

static int SCEDA_edge_hash(SCEDA_Edge *e) {
  return e->id;
}

SCEDA_HashSet *SCEDA_vertex_set_create() {
  return SCEDA_hashset_create(NULL, (SCEDA_match_fun)SCEDA_vertex_match, (SCEDA_hash_fun)SCEDA_vertex_hash);
}

SCEDA_HashMap *SCEDA_vertex_map_create(SCEDA_delete_fun delete) {
  return SCEDA_hashmap_create(NULL, delete, (SCEDA_match_fun)SCEDA_vertex_match, (SCEDA_hash_fun)SCEDA_vertex_hash);
}

SCEDA_HashSet *SCEDA_edge_set_create() {
  return SCEDA_hashset_create(NULL, (SCEDA_match_fun)SCEDA_edge_match, (SCEDA_hash_fun)SCEDA_edge_hash);
}

SCEDA_HashMap *SCEDA_edge_map_create(SCEDA_delete_fun delete) {
  return SCEDA_hashmap_create(NULL, delete, (SCEDA_match_fun)SCEDA_edge_match, (SCEDA_hash_fun)SCEDA_edge_hash);
}

void SCEDA_graph_init(SCEDA_Graph *g, 
		SCEDA_delete_fun delete_vertex_data, 
		SCEDA_delete_fun delete_edge_data) {
  g->counter = 0;
  g->vertices = SCEDA_hashset_create((SCEDA_delete_fun)SCEDA_vertex_delete, (SCEDA_match_fun)SCEDA_vertex_match, (SCEDA_hash_fun)SCEDA_vertex_hash);
  g->edges = SCEDA_hashset_create((SCEDA_delete_fun)SCEDA_edge_delete, (SCEDA_match_fun)SCEDA_edge_match, (SCEDA_hash_fun)SCEDA_edge_hash);
  g->delete_vertex_data = delete_vertex_data;
  g->delete_edge_data = delete_edge_data;
}

SCEDA_Graph *SCEDA_graph_create(SCEDA_delete_fun delete_vertex_data,
		    SCEDA_delete_fun delete_edge_data) {
  SCEDA_Graph *g = (SCEDA_Graph *)safe_malloc(sizeof(SCEDA_Graph));
  SCEDA_graph_init(g, delete_vertex_data, delete_edge_data);
  return g;
}

void SCEDA_graph_cleanup(SCEDA_Graph *g) {
  SCEDA_hashset_delete(g->vertices);
  SCEDA_hashset_delete(g->edges);
  memset(g, 0, sizeof(SCEDA_Graph));
}

void SCEDA_graph_delete(SCEDA_Graph *g) {
  SCEDA_graph_cleanup(g);
  free(g);
}

void SCEDA_graph_clear(SCEDA_Graph *g) {
  SCEDA_delete_fun delete_vertex_data = g->delete_vertex_data;
  SCEDA_delete_fun delete_edge_data = g->delete_edge_data;
  SCEDA_graph_cleanup(g);
  SCEDA_graph_init(g, delete_vertex_data, delete_edge_data);
}

SCEDA_Vertex *SCEDA_graph_add_vertex(SCEDA_Graph *g, const void *data) {
  SCEDA_Vertex *v = (SCEDA_Vertex *)safe_malloc(sizeof(SCEDA_Vertex));

  v->id = g->counter++;

  v->data = (void *)data;
  v->delete = g->delete_vertex_data;

  v->in_edges = SCEDA_vertex_map_create(NULL);
  v->out_edges = SCEDA_vertex_map_create((SCEDA_delete_fun)SCEDA_hashset_delete);

  safe_call(SCEDA_hashset_add(g->vertices, v));

  return v;
}

#include <stdio.h>

int SCEDA_graph_remove_vertex(SCEDA_Graph *g, SCEDA_Vertex *v, void **data) {
  if(SCEDA_hashset_remove(g->vertices, (void **)&v) != 0) {
    return -1;
  }
  
  *data = SCEDA_vertex_get_data(void *, v);

  // delete in edges
  SCEDA_VertexPredIterator pred;
  SCEDA_vertex_pred_iterator_init(v, &pred);
  while(SCEDA_vertex_pred_iterator_has_next(&pred)) {
    SCEDA_Vertex *u = SCEDA_vertex_pred_iterator_next(&pred);
    SCEDA_HashSet *out;
    safe_call(SCEDA_hashmap_remove(u->out_edges, (void **)&v, (void **)&out));

    SCEDA_HashSetIterator edges;
    SCEDA_hashset_iterator_init(out, &edges);
    while(SCEDA_hashset_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_hashset_iterator_next(&edges);
      safe_call(SCEDA_hashset_remove(g->edges, (void **)&e));
      SCEDA_edge_delete(e);
    }
    SCEDA_hashset_iterator_cleanup(&edges);

    SCEDA_hashset_delete(out);
  }
  SCEDA_vertex_pred_iterator_cleanup(&pred);

  // delete out edges
  SCEDA_VertexSuccIterator succ;
  SCEDA_vertex_succ_iterator_init(v, &succ);
  while(SCEDA_vertex_succ_iterator_has_next(&succ)) {
    SCEDA_Vertex *w = SCEDA_vertex_succ_iterator_next(&succ);
    SCEDA_HashSet *out;
    safe_call(SCEDA_hashmap_remove(w->in_edges, (void **)&v, (void **)&out));
    
    SCEDA_HashSetIterator edges;
    SCEDA_hashset_iterator_init(out, &edges);
    while(SCEDA_hashset_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_hashset_iterator_next(&edges);
      safe_call(SCEDA_hashset_remove(g->edges, (void **)&e));
      SCEDA_edge_delete(e);
    }
    SCEDA_hashset_iterator_cleanup(&edges);
  }
  SCEDA_vertex_succ_iterator_cleanup(&succ);

  // free vertex
  SCEDA_vertex_delete_struct(v);

  return 0;
}

SCEDA_Edge *SCEDA_graph_add_edge(SCEDA_Graph *g, SCEDA_Vertex *v_s, SCEDA_Vertex *v_t, const void *data) {
  SCEDA_Edge *e = (SCEDA_Edge *)safe_malloc(sizeof(SCEDA_Edge));

  e->id = g->counter++;

  e->source = v_s;
  e->target = v_t;

  e->data = (void *)data;
  e->delete = g->delete_edge_data;

  safe_call(SCEDA_hashset_add(g->edges, e));

  SCEDA_HashSet *out = SCEDA_hashmap_get(v_s->out_edges, v_t);
  if(out == NULL) {
    out = SCEDA_edge_set_create(NULL);
    safe_call(SCEDA_hashmap_put(v_s->out_edges, v_t, out, NULL));
    safe_call(SCEDA_hashmap_put(v_t->in_edges, v_s, out, NULL));
  }

  safe_call(SCEDA_hashset_add(out, e));

  return e;
}

int SCEDA_graph_remove_edge(SCEDA_Graph *g, SCEDA_Edge *e, void **data) {
  if(SCEDA_hashset_remove(g->edges, (void **)&e) != 0) {
    return -1;
  }
  *data = SCEDA_edge_get_data(void *, e);

  SCEDA_Vertex *v_s = SCEDA_edge_source(e);
  SCEDA_Vertex *v_t = SCEDA_edge_target(e);

  SCEDA_HashSet *out = SCEDA_hashmap_get(v_s->out_edges, v_t);
  safe_ptr(out);
  safe_call(SCEDA_hashset_remove(out, (void **)&e));
  if(SCEDA_hashset_is_empty(out)) {
    safe_call(SCEDA_hashmap_remove(v_s->out_edges, (void **)&v_t, (void **)&out));
    safe_call(SCEDA_hashmap_remove(v_t->in_edges, (void **)&v_s, (void **)&out));
    SCEDA_hashset_delete(out);
  }
  
  SCEDA_edge_delete_struct(e);

  return 0;
}

int SCEDA_vertex_is_succ_of(const SCEDA_Vertex *v_t, const SCEDA_Vertex *v_s) {
  return SCEDA_hashmap_contains_key(v_s->out_edges, v_t);
}

static void SCEDA_out_edges_iterator_aux(SCEDA_OutEdgesIterator *iter) {
  while(SCEDA_hashmap_iterator_has_next(&(iter->succ))) {
    SCEDA_Vertex *v;
    SCEDA_HashSet *out_edges = SCEDA_hashmap_iterator_next(&(iter->succ), &v);
    SCEDA_hashset_iterator_init(out_edges, &(iter->edges));
    if(SCEDA_hashset_iterator_has_next(&(iter->edges))) {
      iter->has_next = TRUE;
      break;
    }
    SCEDA_hashset_iterator_cleanup(&(iter->edges));
  }
}

void SCEDA_out_edges_iterator_init(SCEDA_Vertex *v, SCEDA_OutEdgesIterator *iter) {
  SCEDA_hashmap_iterator_init(v->out_edges, &(iter->succ));
  iter->has_next = FALSE;
  SCEDA_out_edges_iterator_aux(iter);
}

SCEDA_Edge *SCEDA_out_edges_iterator_next(SCEDA_OutEdgesIterator *iter) {
  SCEDA_Edge *e = SCEDA_hashset_iterator_next(&(iter->edges));
  iter->has_next = SCEDA_hashset_iterator_has_next(&(iter->edges));
  if(!iter->has_next) {
    SCEDA_hashset_iterator_cleanup(&(iter->edges));
    SCEDA_out_edges_iterator_aux(iter);
  }
  return e;
}

void SCEDA_out_edges_iterator_cleanup(SCEDA_OutEdgesIterator *iter) {
  SCEDA_hashmap_iterator_cleanup(&(iter->succ));
}

SCEDA_Vertex *SCEDA_vertex_succ_iterator_next(SCEDA_VertexSuccIterator *iter) {
  SCEDA_Vertex *v;
  SCEDA_hashmap_iterator_next(iter, &v);
  return v;
}

static void SCEDA_in_edges_iterator_aux(SCEDA_InEdgesIterator *iter) {
  while(SCEDA_hashmap_iterator_has_next(&(iter->pred))) {
    SCEDA_Vertex *v;
    SCEDA_HashSet *in_edges = SCEDA_hashmap_iterator_next(&(iter->pred), &v);
    SCEDA_hashset_iterator_init(in_edges, &(iter->edges));
    if(SCEDA_hashset_iterator_has_next(&(iter->edges))) {
      iter->has_next = TRUE;
      break;
    }
    SCEDA_hashset_iterator_cleanup(&(iter->edges));
  }
}

void SCEDA_in_edges_iterator_init(SCEDA_Vertex *v, SCEDA_InEdgesIterator *iter) {
  SCEDA_hashmap_iterator_init(v->in_edges, &(iter->pred));
  iter->has_next = FALSE;
  SCEDA_in_edges_iterator_aux(iter);
}

SCEDA_Edge *SCEDA_in_edges_iterator_next(SCEDA_InEdgesIterator *iter) {
  SCEDA_Edge *e = SCEDA_hashset_iterator_next(&(iter->edges));
  iter->has_next = SCEDA_hashset_iterator_has_next(&(iter->edges));
  if(!iter->has_next) {
    SCEDA_hashset_iterator_cleanup(&(iter->edges));
    SCEDA_in_edges_iterator_aux(iter);
  }
  return e;
}

void SCEDA_in_edges_iterator_cleanup(SCEDA_InEdgesIterator *iter) {
  SCEDA_hashmap_iterator_cleanup(&(iter->pred));
}

SCEDA_Vertex *SCEDA_vertex_pred_iterator_next(SCEDA_VertexPredIterator *iter) {
  SCEDA_Vertex *v;
  SCEDA_hashmap_iterator_next(iter, &v);
  return v;
}

static void SCEDA_vertex_adj_iterator_next_aux(SCEDA_VertexAdjIterator *iter) {
  iter->v_adj = NULL;
  while(SCEDA_vertex_pred_iterator_has_next(&(iter->pred))) {
    SCEDA_Vertex *v_adj = SCEDA_vertex_pred_iterator_next(&(iter->pred));
    if(v_adj != iter->v) {
      iter->v_adj = v_adj;
      break;
    }
  }
}

void SCEDA_vertex_adj_iterator_init(SCEDA_Vertex *v, SCEDA_VertexAdjIterator *iter) {
  SCEDA_vertex_pred_iterator_init(v, &(iter->pred));
  SCEDA_vertex_succ_iterator_init(v, &(iter->succ));
  iter->v = v;
  SCEDA_vertex_adj_iterator_next_aux(iter);
}

int SCEDA_vertex_adj_iterator_has_next(SCEDA_VertexAdjIterator *iter) {
  if(iter->v_adj == NULL) {
    return SCEDA_vertex_succ_iterator_has_next(&(iter->succ));
  } else {
    return TRUE;
  }
}

SCEDA_Vertex *SCEDA_vertex_adj_iterator_next(SCEDA_VertexAdjIterator *iter) {
  if(iter->v_adj == NULL) {
    return SCEDA_vertex_succ_iterator_next(&(iter->succ));
  }
  SCEDA_Vertex *v = iter->v_adj;
  SCEDA_vertex_adj_iterator_next_aux(iter);
  return v;
}

void SCEDA_vertex_adj_iterator_cleanup(SCEDA_VertexAdjIterator *iter) {
  SCEDA_vertex_pred_iterator_cleanup(&(iter->pred));
  SCEDA_vertex_succ_iterator_cleanup(&(iter->succ));
  memset(iter, 0, sizeof(SCEDA_VertexAdjIterator));
}

void SCEDA_incident_edges_iterator_next_aux(SCEDA_IncidentEdgesIterator *iter) {
  iter->e = NULL;
  while(SCEDA_in_edges_iterator_has_next(&(iter->in_edges))) {
    SCEDA_Edge *e = SCEDA_in_edges_iterator_next(&(iter->in_edges));
    if(SCEDA_edge_source(e) != SCEDA_edge_target(e)) {
      iter->e = e;
      break;
    }
  }
}

void SCEDA_incident_edges_iterator_init(SCEDA_Vertex *v, SCEDA_IncidentEdgesIterator *iter) {
  SCEDA_in_edges_iterator_init(v, &(iter->in_edges));
  SCEDA_out_edges_iterator_init(v, &(iter->out_edges));
  SCEDA_incident_edges_iterator_next_aux(iter);
}

int SCEDA_incident_edges_iterator_has_next(SCEDA_IncidentEdgesIterator *iter) {
  if(iter->e == NULL) {
    return SCEDA_out_edges_iterator_has_next(&(iter->out_edges));
  } else {
    return TRUE;
  }
}

SCEDA_Edge *SCEDA_incident_edges_iterator_next(SCEDA_IncidentEdgesIterator *iter) {
  if(iter->e == NULL) {
    return SCEDA_out_edges_iterator_next(&(iter->out_edges));
  }
  SCEDA_Edge *e = iter->e;
  SCEDA_incident_edges_iterator_next_aux(iter);
  return e;
}

void SCEDA_incident_edges_iterator_cleanup(SCEDA_IncidentEdgesIterator *iter) {
  SCEDA_in_edges_iterator_cleanup(&(iter->in_edges));
  SCEDA_out_edges_iterator_cleanup(&(iter->out_edges));
  memset(iter, 0, sizeof(SCEDA_IncidentEdgesIterator));
}

void SCEDA_edge_class_iterator_init(SCEDA_Vertex *v_s, SCEDA_Vertex *v_t, SCEDA_EdgeClassIterator *iter) {
  SCEDA_HashSet *all_edges = SCEDA_hashmap_get(v_s->out_edges, v_t);
  if(all_edges != NULL) {
    iter->has_edges = TRUE;
    SCEDA_hashset_iterator_init(all_edges, &(iter->edges));
  } else {
    iter->has_edges = FALSE;
  }
}

int SCEDA_edge_class_iterator_has_next(SCEDA_EdgeClassIterator *iter) {
  if(iter->has_edges) {
    return SCEDA_hashset_iterator_has_next(&(iter->edges));
  } else {
    return FALSE;
  }
}
SCEDA_Edge *SCEDA_edge_class_iterator_next(SCEDA_EdgeClassIterator *iter) {
  return SCEDA_hashset_iterator_next(&(iter->edges));
}
void SCEDA_edge_class_iterator_cleanup(SCEDA_EdgeClassIterator *iter) {
  if(iter->has_edges) {
    SCEDA_hashset_iterator_cleanup(&(iter->edges));
  }
  memset(iter, 0, sizeof(SCEDA_EdgeClassIterator));
}
