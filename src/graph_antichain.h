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
#ifndef SCEDA_GRAPH_ANTICHAIN_H
#define SCEDA_GRAPH_ANTICHAIN_H
/** \file graph_antichain.h
    \brief maximum antichain in a DAG */

#include "graph.h"
#include "list.h"
#include "hashmap.h"

/** Compute a minimum chain cover of the given DAG

    @param gf = graph (must be a DAG)
    @param prev_in_chain = map allocated and computed by the function. It
    gives for each vertex its predecessor in the chain (or NULL).
    @param next_in_chain = map allocated and computed by the function. It
    gives for each vertex its successor in the chain (or NULL). 

    @return the number of chains 

    A topological order must have been computed on gf. */
int SCEDA_graph_minimum_chain_cover(SCEDA_Graph *gf, SCEDA_HashMap **prev_in_chain, SCEDA_HashMap **next_in_chain);

/** Compute a maximum antichain in the given DAG 

    @param g = graph (must be a DAG)

    @return list of vertices in a maximum antichain */
SCEDA_List *SCEDA_graph_maximum_antichain(SCEDA_Graph *g);

#endif
