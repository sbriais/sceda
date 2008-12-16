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

/** \page disjsets Disjoint Sets "à la Tarjan"

    Operations on disjoint sets.

    \section disjsetsapi API

    \code
    void disjoint_set_init(DisjointSet *set, delete_fun delete, void *data);
    DisjointSet *disjoint_set_create(delete_fun delete, void *data);
    \endcode

    Initialise or create a new disjoint set element (a new equivalence
    class). The delete function will be used (if not NULL) upon clean
    up/delete.

    \code
    void disjoint_set_cleanup(DisjointSet *set);
    void disjoint_set_delete(DisjointSet *set);
    \endcode

    Clean up/delete an element. The associated data is deleted
    according to the delete function provided at
    creation/initialisation.

    Beware that all the elements of the same equivalence class may
    become unusable afterwards. They also should be cleaned
    up/deleted.

    \code
    DisjointSet *disjoint_set_find(DisjointSet *set);
    \endcode

    Return the representative element of the equivalence class to
    which the element belongs.

    \code
    void disjoint_set_union(DisjointSet *set_x, DisjointSet *set_y);
    \endcode

    Merge the two equivalence classes to which the two elements
    belong.

    \code
    type disjoint_set_data(type, DisjointSet *set);
    \endcode

    Return the data field of the element.

    \code
    int disjoint_set_equiv(DisjointSet *set_x, DisjointSet *set_y);
    \endcode

    Test whether the two given elements are in the same equivalence
    class.
 */
