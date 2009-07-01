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
#include "listset.h"
#include "util.h"
#include <string.h>

void SCEDA_listset_init(SCEDA_ListSet *set, SCEDA_delete_fun delete, SCEDA_match_fun match) {
  SCEDA_list_init(SCEDA_listset_elements(set), delete);
  set->match = match;
}

SCEDA_ListSet *SCEDA_listset_create(SCEDA_delete_fun delete, SCEDA_match_fun match) {
  SCEDA_ListSet *set = (SCEDA_ListSet *)safe_malloc(sizeof(SCEDA_ListSet));
  SCEDA_listset_init(set, delete, match);
  return set;
}

void SCEDA_listset_cleanup(SCEDA_ListSet *set) {
  SCEDA_list_cleanup(SCEDA_listset_elements(set));
  memset(set, 0, sizeof(SCEDA_ListSet));
}

void SCEDA_listset_delete(SCEDA_ListSet *set) {
  SCEDA_listset_cleanup(set);
  free(set);
}

void SCEDA_listset_clear(SCEDA_ListSet *set) {
  SCEDA_list_clear(SCEDA_listset_elements(set));
}

int SCEDA_listset_add(SCEDA_ListSet *set, const void *data) {
  if(SCEDA_listset_contains(set, data)) {
    return 1;
  } else {
    return SCEDA_list_add(SCEDA_listset_elements(set), data);
  }
}

