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
#ifndef __SCEDA_GRAPH_MRC_H
#define __SCEDA_GRAPH_MRC_H
/** \file graph_mrc.h
    \brief minimum cost-to-time ratio cycle */

#include "graph.h"
#include "list.h"

int SCEDA_graph_minimum_ratio_cycle(SCEDA_Graph *g, 
				    int (*weight)(SCEDA_Edge *e, void *ctxt), void *w_ctxt, 
				    int (*time)(SCEDA_Edge *e, void *ctxt), void *t_ctxt, 
				    int *ratio_num, int *ratio_den,
				    SCEDA_List **min_cycle);

#endif
