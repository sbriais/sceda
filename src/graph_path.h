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
#ifndef __SCEDA_GRAPH_PATH_H
#define __SCEDA_GRAPH_PATH_H
/** \file graph_path.h
    \brief shortest path algorithms */

#include "graph.h"
#include "hashmap.h"

/** PathInfo */
typedef struct {
  /** incoming edge in the path to the vertex */
  SCEDA_Edge *in_edge;
  /** distance from the source of the path: it is infinite (ie there
      is no path) when in_edge = NULL and distance != 0 */
  int distance;
} SCEDA_PathInfo;

/** Compute the shortest paths in a DAG from a unique source.

    @param[in] g = graph (must be a DAG)
    @param[in] from = source of all the paths
    @param[in] dist = distance function
    @param[in] ctxt = distance function context

    @return map of all shortest paths from source vertex

    A topological order must have been computed in g. */
SCEDA_HashMap *SCEDA_graph_shortest_path_from_in_dag(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_int_edge_fun dist, void *ctxt);

/** Compute the shortest paths in a DAG to a unique target.

    @param[in] g = graph (must be a DAG)
    @param[in] to = target of all the paths
    @param[in] dist = distance function
    @param[in] ctxt = distance function context

    @return map of all shortest paths to target vertex

    A topological order must have been computed in g. */
SCEDA_HashMap *SCEDA_graph_shortest_path_to_in_dag(SCEDA_Graph *g, SCEDA_Vertex *to, SCEDA_int_edge_fun dist, void *ctxt);

/** Compute the shortest paths from a unique source using Dijkstra
    algorithm.

    @param[in] g = graph
    @param[in] from = source of all the paths
    @param[in] dist = distance function (negative values are truncated)
    @param[in] ctxt = distance function context

    @return map of all shortest paths from source vertex  */
SCEDA_HashMap *SCEDA_graph_shortest_path_dijkstra(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_int_edge_fun dist, void *ctxt);

/** Compute the shortest paths from a unique source using Bellman-Ford
    algorithm.

    @param[in] g = graph
    @param[in] from = source of all the paths
    @param[in] dist = distance function
    @param[in] ctxt = distance function context
    @param[out] has_neg_cycle = set to TRUE if a negative cycle has been detected, FALSE otherwise

    @return map of all shortest paths from source vertex  */
SCEDA_HashMap *SCEDA_graph_shortest_path_bellman_ford(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_int_edge_fun dist, void *ctxt, int *has_neg_cycle);

#endif
