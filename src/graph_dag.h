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
#ifndef __SCEDA_GRAPH_DAG_H
#define __SCEDA_GRAPH_DAG_H
/** \file graph_dag.h
    \brief Basic algorithms on DAGs */

#include "graph.h"
#include "common.h"

/** Test whether the given graph is acyclic.

    @param[in] g = graph to test 

    @return TRUE if g is acyclic, FALSE otherwise 

    If g is acyclic, indices of a topological order are also set in g */
int SCEDA_graph_is_acyclic(SCEDA_Graph *g);

/** Compute a topological order of the given graph. 

    @param[in] g = graph 

    @return 0 in case of success (ie g is a DAG) and -1 otherwise 

    Indices of a topological order are set in g. 
    Indices go from 0 to |V|-1. */
int SCEDA_graph_compute_topological_order(SCEDA_Graph *g);

/** Compute the transitive closure of the given DAG. 

    @param[in] g = initial graph

    @return the transitive closure of g if g is a DAG. 

    Vertices of g are used to label those of the transitive closure.
    Thus, g should stay alive at least as long as its transitive closure.
    Deleting the transitive closure of g will not delete vertices of g.

    Indices of a topological order are set in g and its transitive closure.

    It will fail if g is not a DAG. */
SCEDA_Graph *SCEDA_graph_transitive_closure(SCEDA_Graph *g);

#endif
