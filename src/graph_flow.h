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

#include "graph.h"
#include "hashmap.h"

SCEDA_HashMap *SCEDA_graph_max_flow(SCEDA_Graph *g, SCEDA_Vertex *s, SCEDA_Vertex *t, int (*capacity)(SCEDA_Edge *e, void *ctxt), void *c_ctxt);

SCEDA_HashMap *SCEDA_graph_min_cost_max_flow(SCEDA_Graph *g, SCEDA_Vertex *s, SCEDA_Vertex *t, int (*capacity)(SCEDA_Edge *e, void *ctxt), void *cap_ctxt, int (*cost)(SCEDA_Edge *e, void *ctxt), void *cost_ctxt);

#endif
