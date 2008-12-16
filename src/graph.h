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
#ifndef SCEDA_GRAPH_H
#define SCEDA_GRAPH_H
/** \file graph.h
    \brief Graph basic definitions. */

#include "hashset.h"
#include "hashmap.h"
#include <string.h>

/** Type of vertex */
typedef struct {
  int id;
  /** Map (target vertex -> out edges) */
  SCEDA_HashMap *out_edges;
  /** Map (source vertex -> out edges) */
  SCEDA_HashMap *in_edges;
  /** Label of the vertex */
  void *data;
  int index;  // mandatory field used by several algorithms
  SCEDA_delete_fun delete;
} SCEDA_Vertex;

/** Create an empty set of vertex. 

    Vertices stored in the set won't be deallocated by hashset_delete.

    Vertices stored in the set must all belong to the same graph.

    @return an empty set of vertex */
SCEDA_HashSet *SCEDA_vertex_set_create();

/** Create a new map indexed by vertice.

    Vertices used as index in the map won't be deallocated by hashmap_delete.

    Vertices used as index in the map must all belong to the same graph.

    @param delete = delete function for map values

    @return an empty map indexed by vertice. */
SCEDA_HashMap *SCEDA_vertex_map_create(SCEDA_delete_fun delete);

/** Set data contained in a vertex.
    
    @param vertex = vertex
    @param data = data 

    \hideinitializer */
#define SCEDA_vertex_set_data(vertex$, data$) ((vertex$)->data = (void *)(data$))

/** Get data contained in a vertex.

    @param type = type of data
    @param vertex = vertex

    @return data 

    \hideinitializer */
#define SCEDA_vertex_get_data(type$, vertex$) ((type$)((vertex$)->data))

/** Type of edge */
typedef struct {
  int id;
  SCEDA_Vertex *source;
  SCEDA_Vertex *target;
  /** Label of the edge */
  void *data;
  SCEDA_delete_fun delete;
} SCEDA_Edge;

/** Type of graph */
typedef struct {
  int counter;
  SCEDA_HashSet *vertices;
  SCEDA_HashSet *edges;
  SCEDA_delete_fun delete_vertex_data;
  SCEDA_delete_fun delete_edge_data;
} SCEDA_Graph;

/** Initialise a graph.

    @param g = graph to initialise
    @param delete_vertex_data = delete function for vertex data
    @param delete_edge_data = delete function for edge data */
void SCEDA_graph_init(SCEDA_Graph *g, 
		   SCEDA_delete_fun delete_vertex_data, 
		   SCEDA_delete_fun delete_edge_data);

/** Create a graph, ready for use.

    @param delete_vertex_data = delete function for vertex data
    @param delete_edge_data = delete function for edge data 

    @return the created graph */
SCEDA_Graph *SCEDA_graph_create(SCEDA_delete_fun delete_vertex_data,
			  SCEDA_delete_fun delete_edge_data);

/** Clean up a graph.

    @param g = graph to delete */
void SCEDA_graph_cleanup(SCEDA_Graph *g);

/** Delete a graph 

    @param g = graph to delete */
void SCEDA_graph_delete(SCEDA_Graph *g);

/** Add a new vertex in a graph.

    @param g = graph
    @param data = label of the new vertex 

    @return the new vertex */
SCEDA_Vertex *SCEDA_graph_add_vertex(SCEDA_Graph *g, const void *data);

/** Remove a vertex from a graph. Every incident edge is deleted.

    @param g = graph
    @param v = vertex to remove
    @param data = label of removed vertex (filled by the function) 

    @return 0 in case of success, -1 otherwise */
int SCEDA_graph_remove_vertex(SCEDA_Graph *g, SCEDA_Vertex *v, void **data);

/** Add a new edge from v_s to v_t.

    @param g = graph
    @param v_s = source vertex
    @param v_t = target vertex
    @param data = label of the edge

    @return the new edge */
SCEDA_Edge *SCEDA_graph_add_edge(SCEDA_Graph *g, SCEDA_Vertex *v_s, SCEDA_Vertex *v_t, const void *data);

/** Remove an edge from a graph. 

    @param g = graph
    @param e = edge to remove
    @param data = label of removed edge (filled by the function) 

    @return 0 in case of success, -1 otherwise */
int SCEDA_graph_remove_edge(SCEDA_Graph *g, SCEDA_Edge *e, void **data);

/** Return the number of vertices in a graph.

    @param g = graph

    @return the number of vertices 

    \hideinitializer */
#define SCEDA_graph_vcount(g$) (SCEDA_hashset_size((g$)->vertices))

/** Return the number of edges in a graph.

    @param g = graph

    @return the number of edges

    \hideinitializer */
#define SCEDA_graph_ecount(g$) (SCEDA_hashset_size((g$)->edges))

/** Get label of a edge 

    @param type = type of label
    @param edge = edge

    @return data 

    \hideinitializer */
#define SCEDA_edge_get_data(type$, edge$) ((type$)((edge$)->data))

/** Get source vertex of a edge.

    @param edge = edge

    @return source vertex

    \hideinitializer */
#define SCEDA_edge_source(edge$) ((edge$)->source)

/** Get target vertex of a edge.

    @param edge = edge
    
    @return target vertex

    \hideinitializer */
#define SCEDA_edge_target(edge$) ((edge$)->target)

/** Return out degree, ie number of successors, of a vertex.

    @param vertex = vertex

    @return out degree

    \hideinitializer */
#define SCEDA_vertex_out_deg(vertex$) (SCEDA_hashmap_size((vertex$)->out_edges))

/** Return in degree, ie number of predecessors, of a vertex.

    @param vertex = vertex

    @return in degree

    \hideinitializer */
#define SCEDA_vertex_in_deg(vertex$) (SCEDA_hashmap_size((vertex$)->in_edges))

#define SCEDA_vertex_get_index(vertex$) ((vertex$)->index)
#define SCEDA_vertex_set_index(vertex$, i$) ((vertex$)->index = (i$))

/** Is v_t a successor of v_s?

    @param v_t = (target) vertex
    @param v_s = (source) vertex 

    @return TRUE if v_t is a successor of v_s, FALSE otherwise */
int SCEDA_vertex_is_succ_of(const SCEDA_Vertex *v_t, const SCEDA_Vertex *v_s);

/** Iterator on all graph vertices. */
typedef SCEDA_HashSetIterator SCEDA_VerticesIterator;

/** Initialise the vertices iterator. 

    @param g = graph
    @param iter = vertices iterator 

    \hideinitializer */
#define SCEDA_vertices_iterator_init(g$, iter$) (SCEDA_hashset_iterator_init((g$)->vertices, (iter$)))

/** Is there a "next" vertex in the iterator?

    \hideinitializer */
#define SCEDA_vertices_iterator_has_next SCEDA_hashset_iterator_has_next

/** Return the next vertex in the iterator.

    @param iter = vertices iterator

    @return the "next" vertex 

    \hideinitializer */
#define SCEDA_vertices_iterator_next(iter$) ((SCEDA_Vertex *)SCEDA_hashset_iterator_next(iter$))

/** Clean up a vertices iterator.

    \hideinitializer */
#define SCEDA_vertices_iterator_cleanup SCEDA_hashset_iterator_cleanup

/** Iterator on all graph edges. */
typedef SCEDA_HashSetIterator SCEDA_EdgesIterator;

/** Initialise the edges iterator. 

    @param g = graph
    @param iter = edges iterator 

    \hideinitializer */
#define SCEDA_edges_iterator_init(g$, iter$) (SCEDA_hashset_iterator_init((g$)->edges, (iter$)))

/** Is there a "next" edge in the iterator?

    \hideinitializer */
#define SCEDA_edges_iterator_has_next SCEDA_hashset_iterator_has_next

/** Return the next edge in the iterator.

    @param iter = edges iterator

    @return the "next" edge 

    \hideinitializer */
#define SCEDA_edges_iterator_next(iter$) ((SCEDA_Edge *)SCEDA_hashset_iterator_next(iter$))

/** Clean up the edges iterator.

    \hideinitializer */
#define SCEDA_edges_iterator_cleanup SCEDA_hashset_iterator_cleanup

/** Iterator on outgoing edges. */
typedef struct {
  SCEDA_HashMapIterator succ;
  int has_next;
  SCEDA_HashSetIterator edges;
} SCEDA_OutEdgesIterator;

/** Initialise the outgoing edges iterator 

    @param v = vertex
    @param iter = outgoing edges iterator to initialise */
void SCEDA_out_edges_iterator_init(SCEDA_Vertex *v, SCEDA_OutEdgesIterator *iter);

/** Is there a "next" edge in the iterator?

    @param iter = out edges iterator
    
    \hideinitializer */
#define SCEDA_out_edges_iterator_has_next(iter$) ((iter$)->has_next)

/** Return the next outgoing edge in the iterator.

    @param iter = outgoing edges iterator

    @return the "next" outgoing edge */
SCEDA_Edge *SCEDA_out_edges_iterator_next(SCEDA_OutEdgesIterator *iter);

/** Clean up the outgoing edges iterator.

    \hideinitializer */
void SCEDA_out_edges_iterator_cleanup(SCEDA_OutEdgesIterator *iter);

/** Iterator on successors. */
typedef SCEDA_HashMapIterator SCEDA_VertexSuccIterator;

/** Initialise the succ iterator.

    @param vertex = vertex
    @param iter = succ iterator to initialise 

    \hideinitializer */
#define SCEDA_vertex_succ_iterator_init(vertex$, iter$) (SCEDA_hashmap_iterator_init((vertex$)->out_edges, (iter$)))

/** Is there a "next" vertex in the succ iterator?

    \hideinitializer */
#define SCEDA_vertex_succ_iterator_has_next SCEDA_hashmap_iterator_has_next

/** Return the "next" vertex in the iterator.

    @param iter = succ iterator

    @return the "next" vertex in the iterator */
SCEDA_Vertex *SCEDA_vertex_succ_iterator_next(SCEDA_VertexSuccIterator *iter);

/** Clean up the succ iterator.

    \hideinitializer */
#define SCEDA_vertex_succ_iterator_cleanup SCEDA_hashmap_iterator_cleanup

/** Iterator on input edges. */
typedef struct {
  SCEDA_HashMapIterator pred;
  int has_next;
  SCEDA_HashSetIterator edges;
} SCEDA_InEdgesIterator;

/** Initialise the input edges iterator 

    @param v = vertex
    @param iter = input edges iterator to initialise */
void SCEDA_in_edges_iterator_init(SCEDA_Vertex *v, SCEDA_InEdgesIterator *iter);

/** Is there a "next" edge in the iterator?

    @param iter = in edges iterator
    
    \hideinitializer */
#define SCEDA_in_edges_iterator_has_next(iter$) ((iter$)->has_next)

/** Return the next input edge in the iterator.

    @param iter = input edges iterator

    @return the "next" input edge */
SCEDA_Edge *SCEDA_in_edges_iterator_next(SCEDA_InEdgesIterator *iter);

/** Clean up the input edges iterator.

    \hideinitializer */
void SCEDA_in_edges_iterator_cleanup(SCEDA_InEdgesIterator *iter);

/** Iterator on predecessors. */
typedef SCEDA_HashMapIterator SCEDA_VertexPredIterator;

/** Initialise the pred iterator.

    @param vertex = vertex
    @param iter = pred iterator to initialise 

    \hideinitializer */
#define SCEDA_vertex_pred_iterator_init(vertex$, iter$) (SCEDA_hashmap_iterator_init((vertex$)->in_edges, (iter$)))

/** Is there a "next" vertex in the pred iterator?

    \hideinitializer */
#define SCEDA_vertex_pred_iterator_has_next SCEDA_hashmap_iterator_has_next

/** Return the "next" vertex in the iterator.

    @param iter = pred iterator

    @return the "next" vertex in the iterator */
SCEDA_Vertex *SCEDA_vertex_pred_iterator_next(SCEDA_VertexPredIterator *iter);

/** Clean up the pred iterator.

    \hideinitializer */
#define SCEDA_vertex_pred_iterator_cleanup SCEDA_hashmap_iterator_cleanup

#endif

