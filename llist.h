#ifndef LLIST_H
#define LLIST_H

#include <stdint.h>

/* Defines */

#define MAX_NAME_LEN                64

#define TRUE                         1
#define FALSE                        0

#define EOK                          0
#define EINVAL                      -1
#define ENOTFOUND                   -2
#define EFAIL                       -3

/* Structure Definitions */

typedef struct llist_elem_ {
    struct llist_elem_   *next;
    struct llist_elem_   *prev;    
} llist_elem_t;

typedef struct llist_ {
    char            list_name[MAX_NAME_LEN];
    llist_elem_t    *list_head;
    llist_elem_t    *list_tail;
    uint32_t        list_offset;
    uint32_t        list_count;
} llist_t;

/* Function prototypes */

llist_t* llist_create (char *name, uint32_t list_offset);
int llist_destroy (llist_t *list);
void* llist_get_head (llist_t *list);
void* llist_get_tail (llist_t *list);
void* llist_get_next (llist_t *list, void *prev_elem);
void* llist_get_prev (llist_t *list, void *next_elem);
uint32_t llist_get_count (llist_t *list);
uint8_t llist_empty (llist_t *list);
int llist_insert (llist_t *list, llist_elem_t *elem);
int llist_insert_head (llist_t *list, llist_elem_t *elem);
int llist_insert_tail (llist_t *list, llist_elem_t *elem);
int llist_insert_after (llist_t *list, llist_elem_t *prev_elem, llist_elem_t *elem);
int llist_insert_before (llist_t *list, llist_elem_t *next_elem, llist_elem_t *elem);
int llist_remove (llist_t *list, llist_elem_t *elem);
void* llist_find (llist_t *list, void *key, int32_t (*cmp_fn)(void *key1, void *key2));
int llist_remove (llist_t *list, llist_elem_t *elem);

#endif /* LIST_H */
