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
#ifndef __SCEDA_GRAPH_DIST_H
#define __SCEDA_GRAPH_DIST_H

#include "graph.h"

/** Type of cost functions */
#define SCEDA_DIST_TYPE double
#include <float.h>
#define SCEDA_DIST_INFTY DBL_MAX

/* #define SCEDA_DIST_TYPE int */
/* #include <limits.h> */
/* #define SCEDA_DIST_INFTY INT_MAX */

typedef SCEDA_DIST_TYPE (*SCEDA_dist_fun)(SCEDA_Edge *e, void *ctxt);

#endif
