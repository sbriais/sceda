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

/** \page sets Sets

    A set is a finite collection of elements, without repetition
    (w.r.t. an equality relation).

    Sets are implemented in three different ways: using either linked
    lists, or hash tables, or balanced binary trees.

    \section listsets_sec ListSet

    ListSet is the worst implementation of sets. It just assumes an
    equality function over set elements.

    Insertion, Removal and Membership test run in linear time O(|set|).

    \section hashsets_sec HashSet

    HashSet is an efficient implementation of random-access sets. In
    addition to an equality function, it assumes a hash function over
    set elements.

    Insertion, Removal and Membership test run in constant time O(1).

    (depends on the hash function, worst case is linear time)

    \section treesets_sec TreeSet

    TreeSet is an efficient implementation of sequential access sets
    (ordered sets). It assumes a comparison function over set
    elements.

    Insertion, Removal and Membership test run in logarithmic time
    O(log|set|).

    \section setsapi_sec API

    The API for sets is described below. 

    Replace the word "set" by either "listset", or "hashset" or
    "treeset" to get the API of the corresponding implementation (see
    also \link listset.h \endlink, \link hashset.h \endlink and \link treeset.h \endlink).
    
    \code
    void set_init(Set *set, delete_function delete, va_list args);
    Set *set_create(delete_function delete, va_list args);
    \endcode

    Initialise or create a set. The delete function provided is used
    (when not NULL) to delete set elements upon
    clean up/delete.

    \code
    void set_cleanup(Set *set);
    void set_delete(Set *set);
    \endcode

    Clean up or delete a set. Delete all the elements in the set
    according to the delete function given at initialisation or creation.

    \code
    void set_clear(Set *set);
    \endcode

    Remove all the element from the set.

    \code
    int set_add(Set *set, const void *data);
    \endcode

    Add an element to a set. Return 0 if the element was
    succesfully added, 1 if the element was already in the set, and -1
    in case of error.

    \code    
    int set_remove(Set *set, void **data);
    \endcode

    Remove an element from a set. 

    Return 0 if the element was successfully removed, 1 if the
    element was not in the set, and -1 in case of error.

    The data is replaced by the data found in the set.

    \code
    int set_contains(Set *set, const void *data);
    \endcode

    Test whether an element belongs to a set. 

    \code
    int set_lookup(Set *set, void **data);
    \endcode

    Search for an element in a set. 

    Return 0 if the element was found and -1 otherwise.

    The data is replaced by the data found in the set.

    \code
    int set_size(Set *set);
    \endcode

    Return the size of the set.

    \code
    int set_is_empty(Set *set);
    \endcode

    Test whether a set is empty.

    \subsection setiter Iterator
    \code
    void set_iterator_init(Set *set, SetIterator *iter);
    \endcode

    Initialise a set iterator. 

    The order in which the elements are visited is not specified for
    list sets and hash sets. On the contrary, the elements are visited
    in ascending order(w.r.t. the comparison function) when dealing
    with tree sets.

    \code
    void set_iterator_cleanup(SetIterator *iter);
    \endcode

    Clean up a set iterator.

    \code
    int set_iterator_has_next(SetIterator *iter);
    \endcode

    Test whether there is a "next" element in a set iterator.

    \code
    void *set_iterator_next(SetIterator *iter);
    \endcode

    Return (consumes) the "next" element of a set iterator.

    Behaviour is unspecified if "has_next" has not been called before or
    if it returned FALSE.

    \section set_examples Examples

    The following three examples create a set of Integers.

    It then print the set content, using a set iterator.

    Finally, the set is deleted.
    
    \subsection listset_exa ListSet example
    
    \code
#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>
#include <SCEDA/listset.h>

typedef struct {
  int value;
} Integer;

Integer *new_Integer(int n) {
  Integer *x = malloc(sizeof(Integer));
  x->value = n;
  return x;
}

void delete_Integer(Integer *x) {
  free(x);
}

int match_Integer(Integer *x, Integer *y) {
  return x->value == y->value;
}

int main(int argc, char *argv[]) {
  // create a set of Integer
  SCEDA_ListSet *set = SCEDA_listset_create((SCEDA_delete_fun)delete_Integer, 
					    (SCEDA_match_fun)match_Integer);
      
  int i;
  for(i = 0; i < 10; i++) {
    int j;
    for(j = 0; j < 2; j++) {
      Integer *x = new_Integer(i);
      // insert x into the set
      if(SCEDA_listset_add(set, x) != 0) {
	// x was not added to the set
	fprintf(stdout,"%d is already in the set\n", x->value);
	// so we delete it
	delete_Integer(x);
      }
    }
  }

  // iterate over the set
  SCEDA_ListSetIterator elts;
  SCEDA_listset_iterator_init(set, &elts);
  while(SCEDA_listset_iterator_has_next(&elts)) {
    // peek the next element
    Integer *x = SCEDA_listset_iterator_next(&elts);
    fprintf(stdout,"%d\n",x->value);
  }
  SCEDA_listset_iterator_cleanup(&elts);

  // delete the set
  SCEDA_listset_delete(set);

  return 0;
}
    \endcode

    \subsection hashset_exa HashSet example

    \code
#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>
#include <SCEDA/hashset.h>

typedef struct {
  int value;
} Integer;

Integer *new_Integer(int n) {
  Integer *x = malloc(sizeof(Integer));
  x->value = n;
  return x;
}

void delete_Integer(Integer *x) {
  free(x);
}

int match_Integer(Integer *x, Integer *y) {
  return x->value == y->value;
}

unsigned int hash_Integer(Integer *x) {
  return x->value;
}

int main(int argc, char *argv[]) {
  // create a set of Integer
  SCEDA_HashSet *set = SCEDA_hashset_create((SCEDA_delete_fun)delete_Integer, 
					    (SCEDA_match_fun)match_Integer,
					    (SCEDA_hash_fun)hash_Integer);
      
  int i;
  for(i = 0; i < 10; i++) {
    int j;
    for(j = 0; j < 2; j++) {
      Integer *x = new_Integer(i);
      // insert x into the set
      if(SCEDA_hashset_add(set, x) != 0) {
	// x was not added to the set
	fprintf(stdout,"%d is already in the set\n", x->value);
	// so we delete it
	delete_Integer(x);
      }
    }
  }

  // iterate over the set
  SCEDA_HashSetIterator elts;
  SCEDA_hashset_iterator_init(set, &elts);
  while(SCEDA_hashset_iterator_has_next(&elts)) {
    // peek the next element
    Integer *x = SCEDA_hashset_iterator_next(&elts);
    fprintf(stdout,"%d\n",x->value);
  }
  SCEDA_hashset_iterator_cleanup(&elts);

  // delete the set
  SCEDA_hashset_delete(set);

  return 0;
}
    \endcode

    \subsection treeeset_exa TreeSet example

    \code
#include <stdio.h>
#include <stdlib.h>
#include <SCEDA/common.h>
#include <SCEDA/treeset.h>

typedef struct {
  int value;
} Integer;

Integer *new_Integer(int n) {
  Integer *x = malloc(sizeof(Integer));
  x->value = n;
  return x;
}

void delete_Integer(Integer *x) {
  free(x);
}

int compare_Integer(Integer *x, Integer *y) {
  if(x->value < y->value) {
    return -1;
  } else if(x->value == y->value) {
    return 0;
  } else {
    return 1;
  }
}

int main(int argc, char *argv[]) {
  // create a set of Integer
  SCEDA_TreeSet *set = SCEDA_treeset_create((SCEDA_delete_fun)delete_Integer, 
					    (SCEDA_compare_fun)compare_Integer);
      
  int i;
  for(i = 0; i < 10; i++) {
    int j;
    for(j = 0; j < 2; j++) {
      Integer *x = new_Integer(i);
      // insert x into the set
      if(SCEDA_treeset_add(set, x) != 0) {
	// x was not added to the set
	fprintf(stdout,"%d is already in the set\n", x->value);
	// so we delete it
	delete_Integer(x);
      }
    }
  }

  // iterate over the set
  SCEDA_TreeSetIterator elts;
  SCEDA_treeset_iterator_init(set, &elts);
  while(SCEDA_treeset_iterator_has_next(&elts)) {
    // peek the next element
    Integer *x = SCEDA_treeset_iterator_next(&elts);
    fprintf(stdout,"%d\n",x->value);
  }
  SCEDA_treeset_iterator_cleanup(&elts);

  // delete the set
  SCEDA_treeset_delete(set);

  return 0;
}
    \endcode
 */
