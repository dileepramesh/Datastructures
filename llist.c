/*
 * llist.c - This file contains a generic doubly linked list implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "llist.h"

/*
 * llist_create
 *
 * Create an empty list and return a pointer to the list
 */
llist_t *
llist_create (char *name, uint32_t offset)
{
    llist_t  *new_list;

    new_list = (llist_t *)malloc(sizeof(llist_t));
    if (!new_list) {
        return NULL;
    }

    /* Initialize the contents */
    strncpy(new_list->list_name, name, strlen(name));
    new_list->list_offset = offset;
    new_list->list_count = 0;
    new_list->list_head = NULL;

    return new_list;
}

/*
 * llist_destroy
 *
 * Free the given list
 */
int
llist_destroy (llist_t *list)
{   
    /* Bail if the list is not empty */
    if (!list_empty(list)) {
        return EFAIL;
    }

    /* Do the deed */
    free(list);

    return EOK; 
}

/*
 * llist_get_head
 *
 * Return a pointer to the head
 */
void *
llist_get_head (llist_t *list)
{
    void *elem = (uint8_t *)list->list_head - list->list_offset;

    return elem;
}

/*
 * llist_get_tail
 *
 * Return a pointer to the tail
 */
void *
llist_get_tail (llist_t *list)
{
    void *elem = (uint8_t *)list->list_tail - list->list_offset;

    return elem;
}

/*
 * llist_get_next
 *
 * Return the next element in the list
 */
void *
llist_get_next (llist_t *list, void *prev_elem)
{
    llist_elem_t *elem;

    /* Sanity check */
    if (!list || !prev_elem) {
        return NULL;
    }

    elem = (llist_elem_t *)((uint8_t *)prev_elem + list->list_offset);
    elem = elem->next;

    if (!elem) {
        return NULL;
    }

    return ((void *)((uint8_t *)elem - list->list_offset));
}

/*
 * llist_get_prev
 *
 * Return the previous element in the list
 */
void *
llist_get_prev (llist_t *list, void *next_elem)
{
    llist_elem_t *elem;

    /* Sanity check */
    if (!list || !next_elem) {
        return NULL;
    }

    elem = (llist_elem_t *)((uint8_t *)next_elem + list->list_offset);
    elem = elem->prev;

    if (!elem) {
        return NULL;
    }

    return ((void *)((uint8_t *)elem - list->list_offset));
}

/*
 * llist_get_count
 *
 * Return the countof elements in the given list
 */
uint32_t
llist_get_count (llist_t *list)
{
    return list->list_count;
}

/*
 * llist_empty
 *
 * Returns true if list is empty. False otherwise
 */
uint8_t
llist_empty (llist_t *list)
{
    if (list->list_count == 0) {
        return TRUE;
    }

    return FALSE;
}

/*
 * llist_insert
 *
 * Insert an element into the tail of the list
 */
int
llist_insert (llist_t *list, llist_elem_t *elem)
{
    llist_elem_t *link;

    /* Sanity check */
    if (!list || !elem) {
        return EINVAL;
    }

    if (list_empty(list)) {
        /* First element */
        list->list_head = elem;
        list->list_tail = elem;
        elem->next = NULL;
        elem->next = NULL;
    } else {
        /* Insert it in the end */
        link = list->list_tail;
        elem->next = NULL;
        elem->prev = link;
        link->next = elem;
        list->list_tail = elem;
    }

    /* Bump up the count */
    list->list_count++;

    return EOK;
}

/*
 * llist_insert_head
 *
 * Insert an element into the head of the list
 */
int
llist_insert_head (llist_t *list, llist_elem_t *elem)
{
    /* Sanity check */
    if (!list || !elem) {
        return EINVAL;
    }

    elem->next = list->list_head;
    elem->prev = NULL;
    list->list_head->prev = elem;
    list->list_head = elem;

    /* Bump up the count */
    list->list_count++;

    return EOK;
}

/*
 * llist_insert_tail
 *
 * Insert an element into the tail of the list
 */
int
llist_insert_tail (llist_t *list, llist_elem_t *elem)
{
    /* Just call list_insert */
    return (list_insert(list, elem));
}

/*
 * llist_insert_after
 *
 * Insert an element after the given element
 */
int 
llist_insert_after (llist_t *list, llist_elem_t *prev_elem, llist_elem_t *elem)
{
    llist_elem_t *link;

    /* Sanity check */
    if (!list || !prev_elem || !elem) {
        return EINVAL;
    }

    link = list->list_head;
    while (link != NULL) {
        if (link == prev_elem) {
            elem->next = prev_elem->next;
            elem->prev = prev_elem;
            prev_elem->next = elem;
            prev_elem->next->prev = elem;

            /* 
             * Update the tail pointer if we are asked to insert after the
             * tail
             */
            if (list->list_tail == prev_elem) {
                list->list_tail = elem;
            }

            /* Bump up the count */
            list->list_count++;

            return EOK;
        }

        link = link->next;
    }

    return ENOTFOUND;
}

/*
 * llist_insert_before
 *
 * Insert an element before the given element
 */
int
llist_insert_before (llist_t *list, llist_elem_t *next_elem, llist_elem_t *elem)
{
    llist_elem_t *link;

    /* Sanity check */
    if (!list || !next_elem || !elem) {
        return EINVAL;
    }

    link = list->list_head;

    /* Are we asked to insert before head? */
    if (link == next_elem) {
        return (list_insert_head(list, elem));
    }

    while (link != NULL) {
        if (link->next == next_elem) {
            elem->next = next_elem;
            elem->prev = link;
            link->next = elem;
            next_elem->prev = elem;

            /* Bump up the count */
            list->list_count++;

            return EOK;
        }

        link = link->next;
    }

    return ENOTFOUND;
}

/*
 * llist_remove
 *
 * Remove an element from the list
 */
int
llist_remove (llist_t *list, llist_elem_t *elem)
{
    llist_elem_t *link;

    /* Sanity check */
    if (!list || !elem) {
        return EINVAL;
    }

    link = list->list_head;

    /* Special case: Are we asked to remove the head? */
    if (link == elem) {
        list->list_head = link->next;
        list->list_head->prev =  NULL;
        list->list_count--;
        return EOK;
    }

    while (link != NULL) {
        if (link->next == elem) {
            link->next = elem->next;
            if (elem->next) {
                elem->next->prev = link;
            }

            /* Are we asked to remove the tail? */
            if (list->list_tail == elem) {
                list->list_tail = link;
            }

            /* Update the count */
            list->list_count--;

            return EOK;
        }

        link = link->next;
    }

    return ENOTFOUND;
}

/*
 * llist_default_cmp_fn
 *
 * Compares two list elements by examining their respective keys.
 * Note that this function just compares the pointers associated
 * with the keys. If the specific key values need to be compared,
 * then the application specific compare function has to be passed.
 */
static int32_t
llist_default_cmp_fn (void *key1, void *key2)
{
    int32_t result = (uint32_t)key1 - (uint32_t)key2;

    return result;
}

/*
 * llist_find
 *
 * This routine tries to lookup an element in the given key using the
 * supplied key and a compare function. If no compare function is
 * passed, the default compare function is used.
 */
void *
llist_find (llist_t *list, void *key, int32_t (*cmp_fn)(void *key1, void *key2))
{
    llist_elem_t *elem;
    void *elem_key;

    /* Sanity check */
    if (!list || !key) {
        return NULL;
    }

    /* Use the default compare function ifnothing is passed */
    if (!cmp_fn) {
        cmp_fn = llist_default_cmp_fn;
    }

    elem = list->list_head;
    while (elem != NULL) {
        elem_key = (void *)((uint8_t *)elem - list->list_offset);

        /* Compare the elements */
        if (cmp_fn(key, elem_key) == 0) {
            /* Match found */
            return elem_key;
        }

        elem = elem->next;
    }

    return NULL;
}

/* End of File */
