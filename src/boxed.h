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
#ifndef __SCEDA_BOXED_H
#define __SCEDA_BOXED_H

#include "util.h"

#define boxed(type$) type$ *

#define boxed_create(type$, v$) \
  ({ boxed(type$) _x = safe_malloc(sizeof(type$)); \
     *_x = (v$);				   \
     _x; })

#define boxed_delete free

#define boxed_set(x$, v$) (*(x$))=(v$)

#define boxed_get(x$) (*(x$))

#endif

