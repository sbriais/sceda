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
#ifndef __SCEDA_GRAPH_MRC_H
#define __SCEDA_GRAPH_MRC_H
/** \file graph_mrc.h
    \brief minimum cost-to-time ratio cycle */

#include "graph.h"
#include "list.h"

/** Compute the minimum ratio cycle of the given graph, ie compute the
    cycle that minimises the ratio cost/time.

    @param[in] g = graph
    @param[in] cost = cost function
    @param[in] c_ctxt = cost function context (closure)
    @param[in] time = time function
    @param[in] t_ctxt = time function context (closure)
    @param[out] ratio_num = numerator of the computed ratio
    @param[out] ratio_den = denominator of the computed ratio
    @param[out] min_cycle = list of the edges of a cycle that minimises the ratio

    @return 0 in case of success, -1 otherwise (and then the list is not allocated) */
int SCEDA_graph_minimum_ratio_cycle(SCEDA_Graph *g, 
				    SCEDA_int_edge_fun cost, void *c_ctxt, 
				    SCEDA_int_edge_fun time, void *t_ctxt, 
				    int *ratio_num, int *ratio_den,
				    SCEDA_List **min_cycle);

#endif
