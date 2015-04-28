/*
   Copyright Sebastien Briais 2008, 2009

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
#ifndef __SCEDA_COMMON_H
#define __SCEDA_COMMON_H
/** \file common.h
    \brief Useful definitions. */

#include <stdlib.h>

/** FALSE */
#ifndef FALSE
#define FALSE 0
#endif

/** TRUE */
#ifndef TRUE
#define TRUE 1
#endif

/** DeFauLT SIZE of all growing/shrinking structures */
#define SCEDA_DFLT_SIZE 10

/** Delete functions. */
typedef void (*SCEDA_delete_fun)(void *);

/** Equality functions.

    @return TRUE=equal, FALSE=different */
typedef int (*SCEDA_match_fun)(const void *, const void *);

#define SCEDA_LT (-1)
#define SCEDA_EQ (0)
#define SCEDA_GT (1)

/** Comparison functions.

    @return
    <0 means "lesser than",
    0 means "equal",
    >0 means "greater than" */
typedef int (*SCEDA_compare_fun)(const void *, const void *);

/** Hash functions. */
typedef unsigned int (*SCEDA_hash_fun)(const void *);

#endif
