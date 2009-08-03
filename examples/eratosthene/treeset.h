#ifndef __SET_H
#define __SET_H

#include <SCEDA/treeset.h>
#define Set SCEDA_TreeSet
#define SetIterator SCEDA_TreeSetIterator

#define set_iterator_init SCEDA_treeset_iterator_init
#define set_iterator_has_next SCEDA_treeset_iterator_has_next
#define set_iterator_next SCEDA_treeset_iterator_next
#define set_iterator_cleanup SCEDA_treeset_iterator_cleanup

#define set_add SCEDA_treeset_add
#define set_size SCEDA_treeset_size
#define set_contains SCEDA_treeset_contains
#define set_remove SCEDA_treeset_remove

#define set_create(delete, match, compare, hash) SCEDA_treeset_create(delete, compare)
#define set_delete SCEDA_treeset_delete

#endif

