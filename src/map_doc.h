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

/** \page maps Maps

    A map is a finite collection of bindings between keys and values.
    A key can be bound to at most one value (i.e. the collection of
    the keys which are bound to a value is a set).

    As for sets, maps are implemented in three different ways: using
    either linked lists, or hash tables, or balanced binary trees.

    \section listmaps_sec ListMap

    ListMap is the worst implementation of maps. It just assumes an
    equality function over keys.

    Insertion, Removal and Find run in linear time O(|map|).

    \section hashmaps_sec HashMap

    HashMap is an efficient implementation of random-access maps. In
    addition to an equality function, it assumes a hash function over
    keys.

    Insertion, Removal and Find run in constant time O(1).

    (depends on the hash function, worst case is linear time)

    \section treemaps_sec TreeMap

    TreeMap is an efficient implementation of sequential access
    maps. It assumes a comparison function over keys.

    Insertion, Removal and Membership test run in logarithmic time
    O(log|map|).

    \section mapsapi_sec API

    The API for maps is described below. 

    Replace the word "map" by either "listmap", or "hashmap" or
    "treemap" to get the API of the corresponding implementation (see
    also \link listmap.h \endlink, \link hashmap.h \endlink and \link treemap.h \endlink).

    \code
    void map_init(Map *map, 
                  delete_fun delete_key, delete_fun delete_value, 
		  va_list args);
    Map *map_create(delete_fun delete_key, delete_fun delete_value, 
                    va_list args);
    \endcode

    Initialise or create a map. The delete_key (resp. delete_value)
    function provided is used (when not NULL) to delete keys
    (resp. values) upon map clean up/delete.

    \code
    void map_cleanup(Map *map);
    void map_delete(Map *map);
    \endcode

    Clean up or delete a map. Delete all the keys and values in the
    map according to the delete functions given at initialisation or
    creation.

    \code
    void map_clear(Map *map);
    \endcode

    Remove all the bindings from a map.

    \code
    int map_put(Map *map, const void *key, const void *value, void **old_value);
    \endcode

    Add (or replace) a binding in a map. Return 0 if the binding has
    been successfully added, 1 if an existing binding was replaced,
    and -1 in case of error.

    If an existing binding was replaced and old_value is not NULL,
    then it is set to the value that was existing in the map before.
    
    \code
    int map_remove(Map *map, void **key, void **value);
    \endcode

    Remove the binding corresponding to a key in a map. 

    Return 0 if a binding was successfully removed, 1 if the
    key was not bound in the map, and -1 in case of error.

    The key (resp. the value) is replaced by the key (resp. the value)
    of the binding found in the map. 

    \code
    int map_contains_key(Map *map, const void *key);
    \endcode

    Test whether a key is bound in a map.

    \code
    void *map_get(Map *map, const void *key);
    \endcode

    Return the value bound to a key in a map or NULL if key is
    unbound. Note that it can also return NULL if the key was bound to
    NULL!

    \code
    int map_lookup(Map *map, void **key, void **value);
    \endcode

    Search for a binding corresponding to a key in a map.

    Return 0 if a binding was found and -1 otherwise.

    The key (resp. the value) is replaced by the key (resp. the value)
    of the binding found in the map. 

    \code
    int map_size(Map *map);
    \endcode

    Return the size of the map.

    \code
    int map_is_empty(Map *map);
    \endcode

    Test whether a map is empty.

    \subsection mapiter_subsec Iterator

    \code
    void map_iterator_init(Map *map, MapIterator *iter);
    \endcode

    Initialise a map iterator. 

    The order in which the bindings are visited is not specified for
    list maps and hash maps. On the contrary, the bindings are visited
    in ascending order (w.r.t. the key comparison function) when dealing
    with tree maps.

    \code
    void map_iterator_cleanup(MapIterator *iter);
    \endcode

    Clean up a map iterator.

    \code
    int map_iterator_has_next(MapIterator *iter);
    \endcode

    Test whether there is a "next" binding in a map iterator.

    \code
    void *map_iterator_next(MapIterator *iter, void **key);
    \endcode

    Return (consumes) the "next" value bound in a set iterator.

    Behaviour is unspecified if "has_next" has not been called before or
    if it returned FALSE.

    The key is replaced by the corresponding key bound to the returned
    value.

    \section map_examples Examples

    The following three examples create a map indexed by strings and
    whose target values are Integers.

    It then print the map content, using a map iterator.

    Finally, the map is deleted.
    
    \subsection listmap_exa ListMap example

    \include "listmap/main.c"

    \subsection hashmap_exa HashMap example

    \include "hashmap/main.c"

    \subsection treemap_exa TreeMap example
    
    \include "treemap/main.c"

*/
