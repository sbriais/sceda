/*
   Copyright 2008, 2009 Sebastien Briais
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
#ifndef __SCEDA_GRAPH_MEANCYCLE_H
#define __SCEDA_GRAPH_MEANCYCLE_H
/** \file graph_meancycle.h
    \brief minimum mean cycle */

#include "graph.h"
#include "list.h"

/** Compute a minimum mean cycle.

    @param[in] g = graph
    @param[in] cost = cost function
    @param[in] c_ctxt = cost function context (closure)
    @param[out] min_cycle = list of the edges of a cycle that minimises the mean cycle

    @return 0 in case of success, -1 otherwise (and then the list is not allocated) */
int SCEDA_graph_minimum_mean_cycle(SCEDA_Graph *g, 
				   SCEDA_int_edge_fun cost, void *c_ctxt, 
				   SCEDA_List **min_cycle);

#endif
