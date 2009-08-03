#ifndef __SET_H
#define __SET_H

#include <SCEDA/hashset.h>
#define Set SCEDA_HashSet
#define SetIterator SCEDA_HashSetIterator

#define set_iterator_init SCEDA_hashset_iterator_init
#define set_iterator_has_next SCEDA_hashset_iterator_has_next
#define set_iterator_next SCEDA_hashset_iterator_next
#define set_iterator_cleanup SCEDA_hashset_iterator_cleanup

#define set_add SCEDA_hashset_add
#define set_size SCEDA_hashset_size
#define set_contains SCEDA_hashset_contains
#define set_remove SCEDA_hashset_remove

#define set_create(delete, match, compare, hash) SCEDA_hashset_create(delete, match, hash)
#define set_delete SCEDA_hashset_delete

#endif
