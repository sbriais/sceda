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
#ifndef __SCEDA_UTIL_H
#define __SCEDA_UTIL_H

#include <stdlib.h>
#include <string.h>

#ifdef UNSAFE
#undef UNSAFE
#define UNSAFE 1
#else 
#define UNSAFE 0
#endif

#if(UNSAFE)

#define safe_malloc(size) (malloc(size))
#define safe_calloc(nmemb, size) (calloc(nmemb, size))
#define safe_realloc(ptr, size) (realloc(ptr, size))

#define safe_strdup(s) (strdup(s))

#define safe_call(x) (x)

#define safe_ptr(x)

#else
#include <assert.h>
#include <stdio.h>

#define fail(msg) \
  ({ fprintf(stderr, "FATAL ERROR: %s (file = %s, line = %d)\n", msg, __FILE__, __LINE__); \
     exit(1); })

#define safe_malloc(size) \
  ({ void *res = malloc(size); \
     if(res == NULL) fail("out of memory"); \
     res; })

#define safe_calloc(nmemb,size)	\
  ({ void *res = calloc(nmemb, size);	    \
     if(res == NULL) fail("out of memory"); \
     res; })

#define safe_realloc(ptr, size) \
  ({ void *res = realloc(ptr, size); \
     if(res == NULL) fail("out of memory"); \
     res; })

#define safe_strdup(s) \
  ({ char *res = strdup(s); \
     if(res == NULL) fail("out of memory"); \
     res; })

#define safe_call(x) \
  ({ if((x) != 0) fail("unsafe call"); })

#define safe_ptr(x) \
  ({ if(x == NULL) fail("null pointer"); })

#endif

#endif
