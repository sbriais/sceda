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
#ifndef __SCEDA_GRAPH_FLOW_H
#define __SCEDA_GRAPH_FLOW_H
/** \file graph_flow.h
    \brief Network Flows algorithms */

#include "graph.h"
#include "hashmap.h"

/** Compute a maximal flow in the directed network.

    @param[in] g = network
    @param[in] s = source
    @param[in] t = sink
    @param[in] capacity = capacity function
    @param[in] c_ctxt = capacity function context
    
    @return a map that tells the amount of flow along each edge or NULL in case of error */
SCEDA_HashMap *SCEDA_graph_max_flow(SCEDA_Graph *g, 
				    SCEDA_Vertex *s, SCEDA_Vertex *t, 
				    SCEDA_int_edge_fun capacity, void *c_ctxt);

/** Compute a maximum flow of minimum cost in the directed network.

    @param[in] g = network
    @param[in] s = source
    @param[in] t = sink
    @param[in] capacity = capacity function
    @param[in] cap_ctxt = capacity function context
    @param[in] cost = cost function
    @param[in] cost_ctxt = cost function context 
   
    @return a map that tells the amount of flow along each edge or NULL in case of error */
SCEDA_HashMap *SCEDA_graph_min_cost_max_flow(SCEDA_Graph *g, 
					     SCEDA_Vertex *s, SCEDA_Vertex *t, 
					     SCEDA_int_edge_fun capacity, void *cap_ctxt, 
					     SCEDA_int_edge_fun cost, void *cost_ctxt);

/** Compute a maximal flow in the directed network that satisfies the supply constraints.

    @param[in] g = network
    @param[in] capacity = capacity function
    @param[in] cap_ctxt = capacity function context
    @param[in] supply = supply function
    @param[in] sup_ctxt = supply function context
    
    @return a map that tells the amount of flow along each edge or NULL in case no feasible flow exists */
SCEDA_HashMap *SCEDA_graph_feasible_flow(SCEDA_Graph *g, 
					 SCEDA_int_edge_fun capacity, void *cap_ctxt, 
					 SCEDA_int_vertex_fun supply, void *sup_ctxt);

/** Compute a minimum cost flow that satisfies the capacity and supply constraints.

    @param[in] g = network
    @param[in] lcap = lower bound capacity function
    @param[in] lcap_ctxt = lower bound capacity function context
    @param[in] ucap = upper bound capacity function
    @param[in] ucap_ctxt = upper bound capacity function context
    @param[in] supply = supply function
    @param[in] sup_ctxt = supply function context
    @param[in] cost = cost function
    @param[in] cost_ctxt = cost function context 

    @return a map that tells the amount of flow along each edge or NULL in case no feasible flow exists

    if lcap == NULL:
    - if lcap_ctxt == NULL then a lower capacity of 0 is assumed.
    - if lcap_ctxt != NULL then a constant lower bound, whose integer
      value is pointed by lcap_ctxt, is assumed.

    if ucap == NULL:
    - if ucap_ctxt == NULL then capacities of edges are unbounded.
    - if lcap_ctxt != NULL then a constant upper bound, whose integer
      value is pointed by ucap_ctxt, is assumed.

    if supply == NULL
    - supply_ctxt is assumed to be a hashmap from vertices to integers (int *)

    if cost == NULL
    - cost_ctxt is assumed to be a hashmap from edges to integers (int *)

    Beware that when capacities are unbounded, the result is correct
    iff the initial graph do not contain a cycle of negative cost and
    unbounded capacity.
*/    
SCEDA_HashMap *SCEDA_graph_min_cost_flow(SCEDA_Graph *g,
					 SCEDA_int_edge_fun lcap, void *lcap_ctxt,
					 SCEDA_int_edge_fun ucap, void *ucap_ctxt,
					 SCEDA_int_vertex_fun supply, void *sup_ctxt,
					 SCEDA_int_edge_fun cost, void *cost_ctxt);

#endif
