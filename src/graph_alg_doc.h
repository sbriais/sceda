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

/** \page graphalgs Graph Algorithms
    
    SCEDA provides implementation of several (classical) graph algorithms.

    \section acycltestsec Acyclicity test

    \code
    int graph_is_acyclic(Graph *g);
    \endcode

    Test whether the given graph in O(|V|+|E|).

    Vertices in g are modified as follows: if g is acyclic, vertice of
    g are indexed according to a topological order of the graph. These
    indices go from 0 to |V|-1.

    Return TRUE if g is acyclic, FALSE otherwise.

    To retrieve the index of a vertex, use the following function:

    \code
    int vertex_get_index(Vertex *v);
    \endcode

    \section topordersec Topological order

    \code
    void graph_compute_topological_order(Graph *g);
    \endcode

    If g is acyclic, vertice of g are attributed an index according to
    a topological order. These indices go from 0 to |V|-1.

    To retrieve the index of a vertex, see \ref acycltestsec.

    Return 0 if g is acyclic, -1 otherwise.

    \section transclossec Transitive closure

    \code
    Graph *graph_transitive_closure(Graph *g);
    \endcode

    If g is acyclic, computes the transitive closure of g. Otherwise,
    results are unpredictable.

    Note that vertices of g are first indexed according to a
    topological order (see \ref topordersec).

    Vertices of the resulting graph are labelled with vertices of g
    (to easily find the corresponding vertex in the original graph).
    
    Edges of the resulting graph are not labelled.

    This is an implementation of Goralcikova-Koubek algorithm.

    \section maxmatchsec Maximum matching in a bipartite graph

    \code
    HashMap *graph_maximum_bipartite_matching(Graph *g, List *x_vert, List *y_vert);
    \endcode

    Computes a maximum matching in a bipartite graph.

    A graph (V,E) is bipartite if there exists a partition of \f$ V =
    X \cup Y \f$ such that each edge goes from X to Y.

    It return a vertex map that gives for each vertex its spouse in
    a maximum matching.

    \section mincoversec Minimum chain cover

    \code
    int graph_minimum_chain_cover(Graph *g, HashMap **prev, HashMap **next);
    \endcode

    Computes a minimum chain cover of the given acyclic graph g.

    As a precondition, a topological order of g should have been
    computed (see \ref topordersec).

    Return the number of chains and creates two maps (prev and next),
    that gives for each vertex, the previous and the next vertex in
    the chain (or NULL if there is none).

    \section maxantichsec Maximum antichain

    \code
    List *graph_maximum_antichain(Graph *g);
    \endcode

    Return a maximum antichain of the given acyclic graph (also know
    as Dilworth decomposition).
    
    \section shortpathdagsec Shortest paths from a source (to a target) in acylic graphs

    \code
    HashMap *graph_shortest_path_from_in_dag(Graph *g, Vertex *from, distance_fun dist, void *dist_data);
    HashMap *graph_shortest_path_to_in_dag(Graph *g, Vertex *to, distance_fun dist, void *dist_data);
    \endcode

    Computes the shortest paths from a source (resp. to a target) in
    an acyclic graph.

    As a precondition, a topological order of g should have been
    computed (see \ref topordersec).

    The distance function gives for each edge the (integer) cost of
    the edge. 

    Return a map that gives for each vertex a PathInfo structure,
    which indicates the previous vertex in the shortest path and the
    cost of the shortest path.

    \code
    HashMap *graph_shortest_path_dijkstra(Graph *g, Vertex *from, distance_fun dist, void *dist_data);
    \endcode

    Computes the shortests paths from a source in a graph, using
    Dijkstra algorithm.

    The distance function gives for each edge the (integer) cost of
    the edge. Since the distance must be positive in Dijkstra
    algorithm, negative values are truncated to zero.

    Return a map that gives for each vertex a PathInfo structure,
    which indicates the previous vertex in the shortest path and the
    cost of the shortest path.

    \code
    HashMap *graph_shortest_path_bellman_ford(Graph *g, Vertex *from, distance_fun dist, void *dist_data, int *has_negative_cycles);
    \endcode

    Computes the shortests paths from a source in a graph, using
    Bellman-Ford algorithm.

    The distance function gives for each edge the (integer) cost of
    the edge. 

    Return a map that gives for each vertex a PathInfo structure,
    which indicates the previous vertex in the shortest path and the
    cost of the shortest path.

    \section graphalgs_examples Examples

    The following example illustrates several of these algorithms on a particular graph.

    \include "graph_alg/main.c"
 */
