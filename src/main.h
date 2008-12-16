/** \mainpage

    \section intro_sec Introduction

    SCEDA (Small Collection of Essential Data structures and
    Algorithms) is intended to be a minimal survivor kit for the C
    programmer. It provides efficient implementation of commonly used
    data structures and algorithms.

    \section ns_sec A word about SCEDA namespace

    In order to not pollute C namespace, all SCEDA functions, macros
    and types are prefixed by the literal "SCEDA_".

    \section main_sec Description of SCEDA

    \li \ref generalities 
    \li \ref lists
    \li \ref sets 
    \li \ref maps
    \li \ref pqueues
    \li \ref heaps
    \li \ref graphs and \ref graphalgs

    \section license_sec License

    (c) 2008 Sebastien Briais

    SCEDA is free software: you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public License
    as published by the Free Software Foundation, either version 3 of
    the License, or (at your option) any later version.

    SCEDA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with SCEDA.  If not, see
    <http://www.gnu.org/licenses/>.
*/

/** \page generalities Using data structures

    The life cycle of a data structure manipulated by SCEDA is:
    <OL>
    <LI> Memory allocation (variable declaration or explicit malloc) </LI>
    <LI> Initialisation of the data structure </LI>
    <LI> Operations on the data structure... </LI>
    <LI> Cleaning up the data structure </LI>
    <LI> Deallocation of memory (function return or explicit free) </LI>
    </OL>

    \section create_subsec Creating and deleting data structures

    Each data structure comes with a "create" function that allocates
    memory in the heap (thanks to malloc) for the data structure and
    initialise it, so that it is ready for use.
    
    The general prototype of "create" function is

    \code
    DataStructure *data_structure_create(va_list args);
    \endcode

    The counterpart of the "create" function is the "delete" function
    that deallocates the memory occupied by the data structure and all
    its subcomponents.

    The general prototype of "delete" function is
   
    \code
    void data_structure_delete(DataStructure *x);
    \endcode
    
    \section init_subsec Initialising and cleaning up data structures

    Each data structure should be initialised before usage: this is
    precisely the goal of the "init" function.

    The general prototype of "init" function is

    \code
    void data_structure_init(DataStructure *x, va_list args);
    \endcode

    The point of having both "create" and "init" function is to allow
    the user to allocate its data structure wherever he wants, and not
    only on the heap. (SCEDA encourages to allocate iterators on
    the stack by not providing "create"/"delete" function).

    The counterpart of the "init" function is the "cleanup" function
    whose goal is to deallocate all auxiliary items used by the data
    structure and reset the state of the data structure as
    uninitialised.

    The general prototype of "cleanup" function is

    \code
    void data_structure_cleanup(DataStructure *x);
    \endcode

    \section gencreate_subsec "create" = "malloc" + "init"
 
    A generic implementation of "create" is the following:

    \code
    DataStructure *data_structure_create(va_list args) {
      DataStructure *x = malloc(sizeof(DataStructure)); 
      assert(x != NULL); // or explode
      data_structure_init(x, args);
      return x;
    }
    \endcode

    \section gendelete_subsec "delete" = "cleanup" + "free"

    A generic implementation of "delete" is the following:

    \code
    void data_structure_delete(DataStructure *x) {
      data_structure_cleanup(x);
      free(x);
    }
    \endcode
*/

