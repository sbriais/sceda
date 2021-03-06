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

/** \page graphalgs Graph Algorithms

    SCEDA provides implementation of several (classical) graph algorithms.

    \section acycltestsec Acyclicity test

    \code
    int graph_is_acyclic(Graph *g);
    \endcode

    Test whether the given graph is acyclic in O(|V|+|E|).

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

    It returns a vertex map that gives for each vertex its spouse in
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

    \section sccsec Strongly connected components

    \code
    List *graph_strongly_connected_components(Graph *g);
    \endcode

    Return the list of the strongly connected components of the given
    graph. A component is encoded as a (hash) set of vertices.

    \section maxantichsec Maximum antichain

    \code
    List *graph_maximum_antichain(Graph *g);
    \endcode

    Return a maximum antichain of the given acyclic graph (also know
    as Dilworth decomposition).

    \section shortpathsec Shortest paths from a source (to a target) in graphs

    \code
    HashMap *graph_shortest_path_from_in_dag(Graph *g, Vertex *from, int_edge_fun dist, void *dist_ctxt);
    HashMap *graph_shortest_path_to_in_dag(Graph *g, Vertex *to, int_edge_fun dist, void *dist_ctxt);
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
    HashMap *graph_shortest_path_dijkstra(Graph *g, Vertex *from, int_edge_fun dist, void *dist_ctxt);
    \endcode

    Computes the shortests paths from a source in a graph, using
    Dijkstra algorithm.

    The distance function gives for each edge the (integer) cost of
    the edge. Since the distance must be positive in Dijkstra
    algorithm, negative values are truncated to zero.

    Return a map that gives for each vertex a PathInfo structure,
    which indicates the incoming edge in the shortest path and the
    cost of the shortest path (from the source).

    \code
    HashMap *graph_shortest_path_bellman_ford(Graph *g, Vertex *from, int_edge_fun dist, void *dist_ctxt, int *has_neg_cycle);
    \endcode

    Computes the shortests paths from a source in a graph, using
    Bellman-Ford algorithm.

    The distance function gives for each edge the (integer) cost of
    the edge.

    Return a map that gives for each vertex a PathInfo structure,
    which indicates the previous vertex in the shortest path and the
    cost of the shortest path.

    Note that if there is negative cycle accessible from source
    vertex, result is incorrect. The variable has_neg_cycle is set to
    TRUE of a negative cycle has been detected and to FALSE otherwise.

    \section mrcsec Minimum Cost to Time Ratio Cycle

    \code
    int graph_minimum_ratio_cycle(Graph *g,
                                  int_edge_fun cost, void *c_ctxt,
				  int_edge_fun time, void *t_ctxt,
				  int *ratio_num, int *ratio_den,
				  List **min_cycle);
    \endcode

    Compute a cycle c that minimises the ratio cost(c)/time(c) and the
    value of this minimum ratio. This function fails when g is acyclic
    (beware that indices of a topological order may be modified), or
    if there is a cycle that has a negative time (ie time(c) <= 0)

    Otherwise, it computes a list of edges belonging to the minimum
    cycle and its ratio.

    It is an implementation of Lawler's algorithm.

    \section meansec Minimum Mean Cycle

    \code
    int graph_minimum_mean_cycle(Graph *g,
                                 int_edge_fun cost, void *c_ctxt,
				 List **min_cycle);
    \endcode

    Compute a cycle that has a minimum mean cost.

    It is an implementation of Karp's algorithm.

    \section flowsec Network Flows

    In the following, a network is a directed graph \f$G = (V, E)\f$.
    We assume to have a lower capacity function \f$lcap: E \mapsto
    Nat\f$, an upper capacity function \f$ucap: E \mapsto Nat\f$, a
    supply function \f$b:V \to ZNat\f$ and a cost function \f$cost: E
    \mapsto Nat\f$.

    A flow is a function \f$f: E \mapsto Nat\f$ such that \f$\forall v
    \in V: \sum\limits_{e \in Out(v)} f(e) - \sum\limits_{e \in In(v)}
    f(e) = b(v)\f$ and \f$\forall e \in E: lcap(e) \leq f(e) \leq
    ucap(e)\f$.

    The cost of a flow is given by \f$\sum\limits_{e \in E} cost(e)
    f(e)\f$.

    Note that a necessary condition for a flow to exist is that
    \f$\sum\limits_{v \in V} b(v) = 0\f$.

    The problem of the maximum flow is the following. Assume to have a
    single source \f$s\f$ and a single sink \f$t\f$.  Assume \f$b(v) =
    0\f$ for any \f$v \in V \setminus \{s, t\}\f$ and \f$lcap(e) =
    0\f$ for any \f$e \in E\f$. Find \f$b(s)\f$ maximum such that a
    flow exists.

    The problem of the feasible flow is the problem of the existence
    of a flow in the simplified case where \f$lcap(e) = 0\f$ for any
    \f$e \in E\f$.

    \code
    HashMap *graph_max_flow(Graph *g, Vertex *s, Vertex *t,
                            int_edge_fun capacity, void *cap_ctxt);
    \endcode

    Compute a maximum flow in the directed network.

    It is an implementation of the highest label push/relabel method.

    \code
    HashMap *graph_min_cost_max_flow(Graph *g, Vertex *s, Vertex *t,
                                     int_edge_fun capacity, void *cap_ctxt,
				     int_edge_fun cost, void *cost_ctxt);
    \endcode

    Compute a maximum flow of minimum cost in the directed
    network.

    \code
    HashMap *graph_feasible_flow(Graph *g,
                                 int_edge_fun capacity, void *cap_ctxt,
				 int_vertex_fun supply, void *sup_ctxt);
    \endcode

    Compute a maximum flow that satisfies the supply constraint, if one exists.

    \code
    HashMap *graph_min_cost_flow(Graph *g,
				 int_edge_fun lcap, void *lcap_ctxt,
				 int_edge_fun ucap, void *ucap_ctxt,
				 int_vertex_fun supply, void *sup_ctxt,
				 int_edge_fun cost, void *cost_ctxt);
    \endcode

    Compute a minimum cost flow that satisfies the capacity and supply
    constraints, if one exists.

    Min Cost Flow algorithms use the cost scaling technique of
    Goldberg & Tarjan.

    \section negcycsec Negative Cycles Detection

    \code
    List *graph_neg_cycle_int(Graph *g, int_edge_fun cost, void *ctxt);
    List *graph_neg_cycle_long_double(Graph *g, long_double_edge_fun cost, void *ctxt);
    \endcode

    Compute a (possibly empty) list of edges belonging to a negative
    cost cycle in the graph.

    The implementation is based upon Bellman-Ford algorithm.

    \section graphalgs_examples Examples

    \subsection graphalgs_mixed Acyclicity test, transitive closure, maximal antichain

    The following example illustrates these algorithms on a particular
    graph.

    \include "graph_alg/main.c"

    \subsection graphalgs_scc Strongly connected components

    \include "graph_scc/main.c"

    \subsection graphalgs_paths Shortest Paths (Bellman-Ford & Dijkstra)

    \include "graph_path/main.c"

    \subsection graphalgs_mrc Minimum Ratio Cycle

    \include "graph_mrc/main.c"

    \subsection graphalgs_mean Minimum Mean Cycle

    \include "graph_meancycle/main.c"

    \subsection graphalgs_maxflow Maximum Flow

    \include "graph_flow/main.c"

    \subsection graphalgs_mincostmaxflow Minimum Cost Flow

    \include "graph_mcf/main.c"

    \subsection graphalgs_negcycle Negative Cycle Detection

 */
