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
#ifndef __SCEDA_GRAPH_PATH_H
#define __SCEDA_GRAPH_PATH_H
/** \file graph_path.h
    \brief shortest path algorithms */

#include "graph.h"
#include "hashmap.h"

/** PathInfo */
typedef struct {
  SCEDA_Vertex *self;
  /** previous vertex in the path */
  SCEDA_Vertex *previous;
  /** distance from the source of the path: it is infinite (ie there
      is no path) when previous = NULL and distance != 0 */
  int distance;
} SCEDA_PathInfo;

/** Type of distance functions */
typedef int (*SCEDA_distance_fun)(SCEDA_Edge *e, void *dist_data);

/** Compute the shortest paths in a DAG from a unique source.

    @param g = graph (must be a DAG)
    @param from = source of all the paths
    @param dist = distance function
    @param dist_ctxt = distance function context

    @return map of all shortest paths from source vertex 

    A topological order must have been computed in g. */
SCEDA_HashMap *SCEDA_graph_shortest_path_from_in_dag(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_distance_fun dist, void *dist_ctxt);

/** Compute the shortest paths in a DAG to a unique target.

    @param g = graph (must be a DAG)
    @param to = target of all the paths
    @param dist = distance function
    @param dist_ctxt = distance function context

    @return map of all shortest paths to target vertex 

    A topological order must have been computed in g. */
SCEDA_HashMap *SCEDA_graph_shortest_path_to_in_dag(SCEDA_Graph *g, SCEDA_Vertex *to, SCEDA_distance_fun dist, void *dist_ctxt);

/** Compute the shortest paths from a unique source using Dijkstra
    algorithm.

    @param g = graph
    @param from = source of all the paths
    @param dist = distance function (negative values are truncated)
    @param dist_ctxt = distance function context

    @return map of all shortest paths from source vertex  */
SCEDA_HashMap *SCEDA_graph_shortest_path_dijkstra(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_distance_fun dist, void *dist_ctxt);    

/** Compute the shortest paths from a unique source using Bellman-Ford
    algorithm.

    @param g = graph
    @param from = source of all the paths
    @param dist = distance function 
    @param dist_ctxt = distance function context
    @param no_negative_cycles = set to FALSE iff there is a cycle of negative weight in the graph

    @return map of all shortest paths from source vertex  */
SCEDA_HashMap *SCEDA_graph_shortest_path_bellman_ford(SCEDA_Graph *g, SCEDA_Vertex *from, SCEDA_distance_fun dist, void *dist_ctxt, int *no_negative_cycles);    

#endif

