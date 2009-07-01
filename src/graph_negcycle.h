/*
   This file is part of SCEDA.
   Copyright 2008, 2009 Sebastien Briais
   
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
#ifndef __SCEDA_GRAPH_NEGCYCLE_H
#define __SCEDA_GRAPH_NEGCYCLE_H
/** \file graph_negcycle.h
    \brief negative cost cycle detection algorithms */

#include "graph.h"
#include "list.h"

/** Compute a (possibly empty) list of edges that are on a negative
    cycle in the given graph.

    @param[in] g = graph
    @param[in] cost = cost function
    @param[in] c_ctxt = cost function context

    @return a list of edges belonging to a negative cost cycle in g.
    The returned list is empty iff there are no negative cycle in g. */
SCEDA_List *SCEDA_graph_neg_cycle_int(SCEDA_Graph *g, SCEDA_int_edge_fun cost, void *c_ctxt);

/** Compute a (possibly empty) list of edges that are on a negative
    cycle in the given graph.

    @param[in] g = graph
    @param[in] cost = cost function
    @param[in] c_ctxt = cost function context

    @return a list of edges belonging to a negative cost cycle in g.
    The returned list is empty iff there are no negative cycle in g. */
SCEDA_List *SCEDA_graph_neg_cycle_long_double(SCEDA_Graph *g, SCEDA_long_double_edge_fun cost, void *c_ctxt);

#endif

