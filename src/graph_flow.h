/*
   This file is part of SCEDA.
   (c) 2008-2009 Sebastien Briais
   
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
    
    @return a map that tells the amount of flow along each edge (ie
    key = edge, data = int *) */
SCEDA_HashMap *SCEDA_graph_max_flow(SCEDA_Graph *g, SCEDA_Vertex *s, SCEDA_Vertex *t, SCEDA_int_edge_fun capacity, void *c_ctxt);

/** Compute a maximal flow of minimum cost in the directed network.

    @param[in] g = network
    @param[in] s = source
    @param[in] t = sink
    @param[in] capacity = capacity function
    @param[in] cap_ctxt = capacity function context
    @param[in] cost = cost function
    @param[in] cost_ctxt = cost function context 
   
    @return a map that tells the amount of flow along each edge (ie
    key = edge, data = int *) */
SCEDA_HashMap *SCEDA_graph_min_cost_max_flow(SCEDA_Graph *g, SCEDA_Vertex *s, SCEDA_Vertex *t, SCEDA_int_edge_fun capacity, void *cap_ctxt, SCEDA_int_edge_fun cost, void *cost_ctxt);

#endif
