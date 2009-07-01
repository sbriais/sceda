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
#ifndef __SCEDA_GRAPH_SCC_H
#define __SCEDA_GRAPH_SCC_H
/** \file graph_scc.h
    \brief strongly connected components */

#include "graph.h"
#include "list.h"

/** Compute the strongly connected components of a graph.

    @param g = graph

    @return a list of hashset (vertex set), each set being a strongly
    connected component of g */
SCEDA_List *SCEDA_graph_strongly_connected_components(SCEDA_Graph *g);

#endif
