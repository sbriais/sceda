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

/** \page graphs Graphs

    A graph G is a couple (V,E,\f$ \phi\f$) where V is a set of
    vertice, E a set of edges and \f$ \phi : E \mapsto V \times V \f$
    gives for each edge its source and its target.

    SCEDA offers support for labeled directed graphs. Each vertex
    and/or edge can have a label. 

    \section graphapi API
    
    \code
    void graph_init(Graph *g, 
                    delete_fun delete_vertex_data, 
		    delete_fun delete_edge_data);

    Graph *graph_create(delete_fun delete_vertex_data,
                        delete_fun delete_edge_data);
    \endcode

    Initialise or create a graph. The delete_vertex_data
    (resp. delete_edge_data) function provided is used (when not NULL)
    to delete vertex labels (resp. edge labels) upon graph clean
    up/delete.
    
    \code
    void graph_cleanup(Graph *g);
    void graph_delete(Graph *g);
    \endcode

    Clean up or delete a graph. Delete all the vertice/edges in the graph
    according to the delete functions given at initialisation or creation.

    \code
    Vertex *graph_add_vertex(Graph *g, void *data);
    \endcode

    Add a new vertex to the graph. Vertex is labelled by given data.

    Return the created vertex.

    \code
    Edge *graph_add_edge(Graph *g, Vertex *source, Vertex *target, void *data);
    \endcode

    Add a new edge to the graph, going from source to target. Edge is
    labelled by given data.

    Return the created edge.

    \code
    Vertex *edge_source(Edge *e);
    Vertex *edge_target(Edge *e);
    \endcode

    Return source (resp. target) vertex of given edge.

    \code
    type vertex_get_data(type, Vertex *v);
    vertex_set_data(Vertex *v, void *data);
    \endcode

    Get/set vertex label (cast to given type).

    \code
    type edge_get_data(type, Edge *e);
    \endcode

    Get edge label (cast to given type).

    \code
    int graph_vcount(Graph *g);
    \endcode

    Return |V|, the number of vertices in the graph.

    \code
    int graph_ecount(Graph *g);
    \endcode

    Return |E|, the number of edges in the graph.

    \code
    int vertex_out_deg(Vertex *v);
    \endcode

    Return the number of successors of a vertex (out degree). 

    \code
    int vertex_in_deg(Vertex *v);
    \endcode
    
    Return the number of predecessors of a vertex (in degree).

    \subsection graphiter Iterators

    Several kinds of iterators are provided. 

    \code
    void vertices_iterator_init(Graph *g, VerticesIterator *iter);
    int vertices_iterator_has_next(VerticesIterator *iter);
    Vertex *vertices_iterator_next(VerticesIterator *iter);
    void vertices_iterator_cleanup(VerticesIterator *iter);
    \endcode

    Iterator over the set of vertices.

    \code
    void edges_iterator_init(Graph *g, EdgesIterator *iter);
    int edges_iterator_has_next(EdgesIterator *iter);
    Edge *edges_iterator_next(EdgesIterator *iter);
    void edges_iterator_cleanup(EdgesIterator *iter);
    \endcode

    Iterator over the set of edges.

    \code
    void vertex_succ_iterator_init(Vertex *v, VertexSuccIterator *iter);
    int vertex_succ_iterator_has_next(VertexSuccIterator *iter);
    Vertex *vertex_succ_iterator_next(VertexSuccIterator *iter);
    void vertex_succ_iterator_cleanup(VertexSuccIterator *iter);
    \endcode

    Iterator over the set of successors of a vertex v.

    \code
    void out_edges_iterator_init(Vertex *v, OutEdgesIterator *iter);
    int out_edges_iterator_has_next(OutEdgesIterator *iter);
    Edge *out_edges_iterator_next(OutEdgesIterator *iter);
    void out_edges_iterator_cleanup(OutEdgesIterator *iter);
    \endcode

    Iterator over the set of outgoing edges from a vertex v.

    \code
    void vertex_pred_iterator_init(Vertex *v, VertexPredIterator *iter);
    int vertex_pred_iterator_has_next(VertexPredIterator *iter);
    Vertex *vertex_pred_iterator_next(VertexPredIterator *iter);
    void vertex_pred_iterator_cleanup(VertexPredIterator *iter);
    \endcode

    Iterator over the set of predecessors of a vertex v. 

    \code
    void in_edges_iterator_init(Vertex *v, InEdgesIterator *iter);
    int in_edges_iterator_has_next(InEdgesIterator *iter);
    Edge *in_edges_iterator_next(InEdgesIterator *iter);
    void in_edges_iterator_cleanup(InEdgesIterator *iter);
    \endcode

    Iterator over the set of input edges to a vertex v. 

    \code
    void vertex_adj_iterator_init(Vertex *v, VertexAdjIterator *iter);
    int vertex_adj_iterator_has_next(VertexAdjIterator *iter);
    Vertex *vertex_adj_iterator_next(VertexAdjIterator *iter);
    void vertex_adj_iterator_cleanup(VertexAdjIterator *iter);
    \endcode

    Iterator over the set of adjacent vertices (predecessors and
    successors) of a vertex v.

    \code
    void incident_edges_iterator_init(Vertex *v, IncidentEdgesIterator *iter);
    int incident_edges_iterator_has_next(IncidentEdgesIterator *iter);
    Edge *incident_edges_iterator_next(IncidentEdgesIterator *iter);
    void incident_edges_iterator_cleanup(IncidentEdgesIterator *iter);
    \endcode

    Iterator over the set of incident edges (input and outgoing edges)
    to a vertex v.

    \section graph_examples Example

    The following example creates a graph whose nodes are labelled by
    Integers and whose edges are not labelled (NULL).

    Then it prints the graph and finally delete it.

    \include "graph/main.c"
 */
