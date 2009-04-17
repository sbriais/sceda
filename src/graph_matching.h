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
#ifndef __SCEDA_GRAPH_MATCHING_H
#define __SCEDA_GRAPH_MATCHING_H
/** \file graph_matching.h
    \brief maximum matching in a bipartite graph */

#include "graph.h"
#include "list.h"

/** Compute a maximum matching in the bipartite graph G.
    Edges are assumed to go from X to Y.

    @param[in] g = bipartite graph
    @param[in] x_vert = first set of the bipartition
    @param[in] y_vert = second set of the bipartition

    @return a map which give for each vertex of X or Y its spouse in the other set. */
SCEDA_HashMap *SCEDA_graph_maximum_bipartite_matching(SCEDA_Graph *g, SCEDA_List *x_vert, SCEDA_List *y_vert);

#endif
