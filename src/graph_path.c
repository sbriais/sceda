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
#include "graph.h"
#include "graph_dag.h"
#include "graph_path.h"
#include "hashmap.h"
#include "common.h"
#include "util.h"
#include "heap.h"
#include "queue.h"

/* #define SCEDA_DIST_TYPE long double */
/* #include <float.h> */
/* #define SCEDA_DIST_INFTY DBL_MAX */

#define SCEDA_DIST_TYPE int
#include <limits.h>
#define SCEDA_DIST_INFTY INT_MAX
#define SCEDA_dist_fun SCEDA_int_edge_fun

#define INFINITY SCEDA_DIST_INFTY

static SCEDA_PathInfo *SCEDA_path_info_create(SCEDA_Vertex *source, SCEDA_Vertex *u) {
  SCEDA_PathInfo *info = (SCEDA_PathInfo *)safe_malloc(sizeof(SCEDA_PathInfo));
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

static inline int SCEDA_path_relax(SCEDA_PathInfo *info_u, SCEDA_PathInfo *info_v, SCEDA_DIST_TYPE weight, SCEDA_Edge *in) {
  if(is_infty(info_u)) {
    return FALSE;
  }
  SCEDA_DIST_TYPE du = info_u->distance;
  if((is_infty(info_v)) || (du+weight < info_v->distance)) {
/*     if(!is_infty(info_v)) { */
/*       fprintf(stderr,"%Lf + %Lf = %Lf < %Lf\n",du,weight,du+weight,info_v->distance); */
/*     } */
    info_v->distance = du + weight;
    info_v->in_edge = in;
    return TRUE;
  }
  return FALSE;
}

SCEDA_HashMap *SCEDA_graph_shortest_path_from_in_dag(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_dist_fun dist, void *ctxt) {
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

SCEDA_HashMap *SCEDA_graph_shortest_path_to_in_dag(SCEDA_Graph *g, SCEDA_Vertex *to, SCEDA_dist_fun dist, void *ctxt) {
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

SCEDA_HashMap *SCEDA_graph_shortest_path_dijkstra(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_dist_fun dist, void *ctxt) {
  SCEDA_HashMap *paths = SCEDA_vertex_map_create((SCEDA_delete_fun)SCEDA_path_info_delete);
  SCEDA_HashMap *elts = SCEDA_vertex_map_create(NULL);
  SCEDA_Heap *heap = SCEDA_heap_create(NULL, NULL, (SCEDA_compare_fun)SCEDA_path_info_compare);

  SCEDA_VerticesIterator g_vertice;
  SCEDA_vertices_iterator_init(g, &g_vertice);
  while(SCEDA_vertices_iterator_has_next(&g_vertice)) {
    SCEDA_Vertex *v = SCEDA_vertices_iterator_next(&g_vertice);
    SCEDA_PathInfo *info = SCEDA_path_info_create(from, v);
    SCEDA_hashmap_put(paths, v, info, NULL);
    SCEDA_HeapElt *elt = SCEDA_heap_insert(heap, v, info);
    SCEDA_hashmap_put(elts, v, elt, NULL);
  }
  SCEDA_vertices_iterator_cleanup(&g_vertice);

  while(!SCEDA_heap_is_empty(heap)) {
    SCEDA_Vertex *u;
    SCEDA_PathInfo *info_u;
    SCEDA_heap_extract(heap, (void **)&u, (void **)&info_u);

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

SCEDA_HashMap *SCEDA_graph_shortest_path_bellman_ford(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_dist_fun dist, void *ctxt, int *has_cycle) {
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

  SCEDA_HashSet *in_queue = SCEDA_vertex_set_create();

  SCEDA_Queue *queue = SCEDA_queue_create(NULL);
  safe_call(SCEDA_queue_enqueue(queue, from));
  safe_call(SCEDA_queue_enqueue(queue, NULL));
  safe_call(SCEDA_hashset_add(in_queue, from));

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

    SCEDA_PathInfo *info_u = SCEDA_hashmap_get(paths, u);

    SCEDA_OutEdgesIterator out_edges;
    SCEDA_out_edges_iterator_init(u, &out_edges);
    while(SCEDA_out_edges_iterator_has_next(&out_edges)) {
      SCEDA_Edge *e = SCEDA_out_edges_iterator_next(&out_edges);
      SCEDA_Vertex *v = SCEDA_edge_target(e);
      SCEDA_PathInfo *info_v = SCEDA_hashmap_get(paths, v);
      if(SCEDA_path_relax(info_u, info_v, dist(e, ctxt), e)) {
	if(!SCEDA_hashset_contains(in_queue, v)) {
	  safe_call(SCEDA_queue_enqueue(queue, v));
	  safe_call(SCEDA_hashset_add(in_queue, v));
	}
      }
    }
    SCEDA_out_edges_iterator_cleanup(&out_edges);
  }

  SCEDA_hashset_delete(in_queue);

  if((i < n) || (SCEDA_queue_is_empty(queue))) {
    *has_cycle = FALSE;
  } else {
    *has_cycle = TRUE;
  }

  SCEDA_queue_delete(queue);
  return paths;
}
