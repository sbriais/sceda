#ifndef __SET_H
#define __SET_H

#include <SCEDA/listset.h>
#define Set SCEDA_ListSet
#define SetIterator SCEDA_ListSetIterator

#define set_iterator_init SCEDA_listset_iterator_init
#define set_iterator_has_next SCEDA_listset_iterator_has_next
#define set_iterator_next SCEDA_listset_iterator_next
#define set_iterator_cleanup SCEDA_listset_iterator_cleanup

#define set_add SCEDA_listset_add
#define set_size SCEDA_listset_size
#define set_contains SCEDA_listset_contains
#define set_remove SCEDA_listset_remove

#define set_create(delete, match, compare, hash) SCEDA_listset_create(delete, match)
#define set_delete SCEDA_listset_delete

#endif
