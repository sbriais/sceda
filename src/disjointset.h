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
#ifndef __SCEDA_DISJOINTSET_H
#define __SCEDA_DISJOINTSET_H
/** \file disjointset.h
    \brief Disjoint Sets "à la Tarjan" 
    (implements efficient union-find operations) */

#include "common.h"

/** Type of disjoint sets */
typedef struct _SCEDA_DisjointSet {
  struct _SCEDA_DisjointSet *repr;
  int rank;
  void *data;
  SCEDA_delete_fun delete;
} SCEDA_DisjointSet;

/** Initialise a disjoint set (new class of equivalence)

    @param set = disjoint set to initialise
    @param delete = delete function or NULL
    @param data = data in the set */
void SCEDA_disjoint_set_init(SCEDA_DisjointSet *set, SCEDA_delete_fun delete, void *data);

/** Create a disjoint set.

    @param delete = delete function or NULL
    @param data = data in the set

    @return the disjoint set */
SCEDA_DisjointSet *SCEDA_disjoint_set_create(SCEDA_delete_fun delete, void *data);

/** Clean up a disjoint set.

    @param set = disjoint set */
void SCEDA_disjoint_set_cleanup(SCEDA_DisjointSet *set);

/** Delete a disjoint set. */
void SCEDA_disjoint_set_delete(SCEDA_DisjointSet *set);

/** Find the representative in disjoint set equivalence class.

    @param set = disjoint set

    @return the representative */
SCEDA_DisjointSet *SCEDA_disjoint_set_find(SCEDA_DisjointSet *set); 

/** Union of two disjoint sets equivalence classes.

    @param set_x = first set
    @param set_y = second set */
void SCEDA_disjoint_set_union(SCEDA_DisjointSet *set_x, SCEDA_DisjointSet *set_y);

/** Get data associated to the class element.

    @param type = type of data
    @param set = disjoint set (class element) 

    @return the data */
#define SCEDA_disjoint_set_data(type$, set$) ((type$)((set$)->data))

/** Test whether two disjoint sets are in the same class.

    @param set_x = first set
    @param set_y = second set

    @return TRUE if the two sets are in the same equivalence class,
    FALSE otherwise */
int SCEDA_disjoint_set_equiv(SCEDA_DisjointSet *set_x, SCEDA_DisjointSet *set_y);

#endif
