/*
   This file is part of SCEDA.
   (c) 2008 Sebastien Briais
   
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
#include "graph.h"
#include "graph_dag.h"
#include "graph_path.h"
#include "hashmap.h"
#include "common.h"
#include "util.h"
#include "heap.h"

#define INFINITY SCEDA_COST_INFTY

static SCEDA_PathInfo *SCEDA_path_info_create(SCEDA_Vertex *source, SCEDA_Vertex *u) {
  SCEDA_PathInfo *info = (SCEDA_PathInfo *)safe_malloc(sizeof(SCEDA_PathInfo));
  info->self = u;
  info->in_edge = NULL;
  if(u == source) {
    info->distance = 0;
  } else {
    info->distance = INFINITY;
  }
  return info;
}

static void SCEDA_path_info_delete(SCEDA_PathInfo *info) {
  free(info);
}

#define is_infty(info$) (((info$)->in_edge == NULL) && ((info$)->distance != 0))

static int SCEDA_path_info_compare(SCEDA_PathInfo *info1, SCEDA_PathInfo *info2) {
  if(is_infty(info1)) {
    if(is_infty(info2)) {
      return 0;
    } else {
      return 1;
    }
  }
  if(is_infty(info2)) {
    return -1;
  }
  int d1 = info1->distance;
  int d2 = info2->distance;
  if(d1 < d2) {
    return -1;
  } else if(d1 == d2) {
    return 0;
  } else {
    return 1;
  }
}

static inline int SCEDA_path_relax(SCEDA_PathInfo *info_u, SCEDA_PathInfo *info_v, SCEDA_COST_TYPE weight, SCEDA_Edge *in) {
  if(is_infty(info_u)) {
    return FALSE;
  }
  SCEDA_COST_TYPE du = info_u->distance;
  if((is_infty(info_v)) || (du+weight < info_v->distance)) {
    info_v->distance = du + weight;
    info_v->in_edge = in;
    return TRUE;
  }
  return FALSE;
}

SCEDA_HashMap *SCEDA_graph_shortest_path_from_in_dag(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_cost_fun dist, void *ctxt) {
  SCEDA_HashMap *paths = SCEDA_vertex_map_create((SCEDA_delete_fun)SCEDA_path_info_delete);

  int n = SCEDA_graph_vcount(g);
  SCEDA_Vertex *vertice[n];

  SCEDA_VerticesIterator g_vertice;
  SCEDA_vertices_iterator_init(g, &g_vertice);
  while(SCEDA_vertices_iterator_has_next(&g_vertice)) {
    SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&g_vertice);
    int i = SCEDA_vertex_get_index(v);
    vertice[i] = v;
    SCEDA_PathInfo *info = SCEDA_path_info_create(from, v);
    SCEDA_hashmap_put(paths, v, info, NULL);
  }
  SCEDA_vertices_iterator_cleanup(&g_vertice);

  int i;
  for(i = 0; i < n; i++) {
    SCEDA_Vertex *u = vertice[i];
    SCEDA_PathInfo *info_u = SCEDA_hashmap_get(paths, u);
    SCEDA_OutEdgesIterator out_edges;
    SCEDA_out_edges_iterator_init(u, &out_edges);
    while(SCEDA_out_edges_iterator_has_next(&out_edges)) {
      SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&out_edges);
      SCEDA_Vertex *v = SCEDA_edge_target(e);
      SCEDA_PathInfo *info_v = SCEDA_hashmap_get(paths, v);
      SCEDA_path_relax(info_u, info_v, dist(e, ctxt), e);
    }
    SCEDA_out_edges_iterator_cleanup(&out_edges);
  }

  return paths;
}

SCEDA_HashMap *SCEDA_graph_shortest_path_to_in_dag(SCEDA_Graph *g, SCEDA_Vertex *to, SCEDA_cost_fun dist, void *ctxt) {
  SCEDA_HashMap *paths = SCEDA_vertex_map_create((SCEDA_delete_fun)SCEDA_path_info_delete);

  int n = SCEDA_graph_vcount(g);
  SCEDA_Vertex *vertice[n];

  SCEDA_VerticesIterator g_vertice;
  SCEDA_vertices_iterator_init(g, &g_vertice);
  while(SCEDA_vertices_iterator_has_next(&g_vertice)) {
    SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&g_vertice);
    int i = SCEDA_vertex_get_index(v);
    vertice[i] = v;
    SCEDA_PathInfo *info = SCEDA_path_info_create(to, v);
    SCEDA_hashmap_put(paths, v, info, NULL);
  }
  SCEDA_vertices_iterator_cleanup(&g_vertice);

  int i;
  for(i = n-1; i >= 0; i--) {
    SCEDA_Vertex *u = vertice[i];
    SCEDA_PathInfo *info_u = SCEDA_hashmap_get(paths, u);
    SCEDA_InEdgesIterator in_edges;
    SCEDA_in_edges_iterator_init(u, &in_edges);
    while(SCEDA_in_edges_iterator_has_next(&in_edges)) {
      SCEDA_Edge *e = SCEDA_in_edges_iterator_next(&in_edges);
      SCEDA_Vertex *v = SCEDA_edge_source(e);
      SCEDA_PathInfo *info_v = SCEDA_hashmap_get(paths, v);
      SCEDA_path_relax(info_u, info_v, dist(e, ctxt), e);
    }
    SCEDA_in_edges_iterator_cleanup(&in_edges);
  }

  return paths;
}

SCEDA_HashMap *SCEDA_graph_shortest_path_dijkstra(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_cost_fun dist, void *ctxt) {
  SCEDA_HashMap *paths = SCEDA_vertex_map_create((SCEDA_delete_fun)SCEDA_path_info_delete);
  SCEDA_HashMap *elts = SCEDA_vertex_map_create(NULL);
  SCEDA_Heap *heap = SCEDA_heap_create(NULL, (SCEDA_compare_fun)SCEDA_path_info_compare);

  SCEDA_VerticesIterator g_vertice;
  SCEDA_vertices_iterator_init(g, &g_vertice);
  while(SCEDA_vertices_iterator_has_next(&g_vertice)) {
    SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&g_vertice);
    SCEDA_PathInfo *info = SCEDA_path_info_create(from, v);
    SCEDA_hashmap_put(paths, v, info, NULL);
    SCEDA_HeapElt *elt = SCEDA_heap_insert(heap, info);
    SCEDA_hashmap_put(elts, v, elt, NULL);
  }
  SCEDA_vertices_iterator_cleanup(&g_vertice);

  while(!SCEDA_heap_is_empty(heap)) {
    SCEDA_PathInfo *info_u;
    SCEDA_heap_extract(heap, (void **)&info_u);
    SCEDA_Vertex *u = info_u->self;

    SCEDA_OutEdgesIterator out_edges;
    SCEDA_out_edges_iterator_init(u, &out_edges);
    while(SCEDA_out_edges_iterator_has_next(&out_edges)) {
      SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&out_edges);
      SCEDA_Vertex *v = SCEDA_edge_target(e);
      SCEDA_PathInfo *info_v = SCEDA_hashmap_get(paths, v);

      int de = dist(e, ctxt);
      if(de < 0) {
	de = 0;
      }

      if(SCEDA_path_relax(info_u, info_v, de, e)) {
	SCEDA_HeapElt *elt = SCEDA_hashmap_get(elts, v);
	SCEDA_heap_decrease_key(heap, elt);
      }
    }
    SCEDA_out_edges_iterator_cleanup(&out_edges);
  }

  SCEDA_hashmap_delete(elts);
  SCEDA_heap_delete(heap);

  return paths;
}

SCEDA_HashMap *SCEDA_graph_shortest_path_bellman_ford(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_cost_fun dist, void *ctxt, SCEDA_Vertex **neg_cycle) {
  SCEDA_HashMap *paths = SCEDA_vertex_map_create((SCEDA_delete_fun)SCEDA_path_info_delete);

  int n = SCEDA_graph_vcount(g);

  SCEDA_VerticesIterator g_vertice;
  SCEDA_vertices_iterator_init(g, &g_vertice);
  while(SCEDA_vertices_iterator_has_next(&g_vertice)) {
    SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&g_vertice);
    SCEDA_PathInfo *info = SCEDA_path_info_create(from, v);
    SCEDA_hashmap_put(paths, v, info, NULL);
  }
  SCEDA_vertices_iterator_cleanup(&g_vertice);

  int i;
  for(i = 1; i < n; i++) {
    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
      SCEDA_Vertex *u = SCEDA_edge_source(e);
      SCEDA_Vertex *v = SCEDA_edge_target(e);
      SCEDA_PathInfo *info_u = SCEDA_hashmap_get(paths, u);
      SCEDA_PathInfo *info_v = SCEDA_hashmap_get(paths, v);
      
      SCEDA_path_relax(info_u, info_v, dist(e, ctxt), e);
    }
    SCEDA_edges_iterator_cleanup(&edges);
  }

  if(neg_cycle != NULL) {
    *neg_cycle = NULL;

    SCEDA_EdgesIterator edges;
    SCEDA_edges_iterator_init(g, &edges);
    while(SCEDA_edges_iterator_has_next(&edges)) {
      SCEDA_Edge *e = SCEDA_edges_iterator_next(&edges);
      SCEDA_Vertex *u = SCEDA_edge_source(e);
      SCEDA_Vertex *v = SCEDA_edge_target(e);
      SCEDA_PathInfo *info_u = SCEDA_hashmap_get(paths, u);
      SCEDA_PathInfo *info_v = SCEDA_hashmap_get(paths, v);
      if(SCEDA_path_relax(info_u, info_v, dist(e, ctxt), e)) {
	*neg_cycle = u;
	break;
      }
    }
    SCEDA_edges_iterator_cleanup(&edges);

    if(*neg_cycle != NULL) {
      SCEDA_HashSet *cycle = SCEDA_vertex_set_create();
      do {
	safe_call(SCEDA_hashset_add(cycle, *neg_cycle));
	SCEDA_PathInfo *info = SCEDA_hashmap_get(paths, *neg_cycle);
	safe_ptr(info->in_edge);
	*neg_cycle = SCEDA_edge_source(info->in_edge);
      } while(!SCEDA_hashset_contains(cycle, *neg_cycle));
      SCEDA_hashset_delete(cycle);
    }
  }

  return paths;
}
