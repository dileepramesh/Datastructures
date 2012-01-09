#ifndef LIST_H
#define LIST_H

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

typedef struct list_elem_ {
    struct list_elem_   *next;
} list_elem_t;

typedef struct list_ {
    char            list_name[MAX_NAME_LEN];
    list_elem_t     *list_head;
    list_elem_t     *list_tail;
    uint32_t        list_offset;
    uint32_t        list_count;
} list_t;

/* Function prototypes */

list_t* list_create (char *name, uint32_t list_offset);
int list_destroy (list_t *list);
void* list_get_head (list_t *list);
void* list_get_tail (list_t *list);
void* list_get_next (list_t *list, void *prev_elem);
uint32_t list_get_count (list_t *list);
uint8_t list_empty (list_t *list);
int list_insert (list_t *list, list_elem_t *elem);
int list_insert_head (list_t *list, list_elem_t *elem);
int list_insert_tail (list_t *list, list_elem_t *elem);
int list_insert_after (list_t *list, list_elem_t *prev_elem, list_elem_t *elem);
int list_insert_before (list_t *list, list_elem_t *next_elem, list_elem_t *elem);
int list_remove (list_t *list, list_elem_t *elem);
void* list_find (list_t *list, void *key, int32_t (*cmp_fn)(void *key1, void *key2));
int list_remove (list_t *list, list_elem_t *elem);

#endif /* LIST_H */
