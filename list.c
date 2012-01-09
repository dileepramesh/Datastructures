/*
 * list.c - This file contains a generic linked list implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/*
 * list_create
 *
 * Create an empty list and return a pointer to the list
 */
list_t *
list_create (char *name, uint32_t offset)
{
    list_t  *new_list;

    new_list = (list_t *)malloc(sizeof(list_t));
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
 * list_destroy
 *
 * Free the given list
 */
int
list_destroy (list_t *list)
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
 * list_get_head
 *
 * Return a pointer to the head
 */
void *
list_get_head (list_t *list)
{
    void *elem = (uint8_t *)list->list_head - list->list_offset;

    return elem;
}

/*
 * list_get_tail
 *
 * Return a pointer to the tail
 */
void *
list_get_tail (list_t *list)
{
    void *elem = (uint8_t *)list->list_tail - list->list_offset;

    return elem;
}

/*
 * list_get_next
 *
 * Return the next element in the list
 */
void *
list_get_next (list_t *list, void *prev_elem)
{
    list_elem_t *elem;

    /* Sanity check */
    if (!list || !elem) {
        return NULL;
    }

    elem = (list_elem_t *)((uint8_t *)prev_elem + list->list_offset);
    elem = elem->next;

    if (!elem) {
        return NULL;
    }

    return ((void *)((uint8_t *)elem - list->list_offset));
}

/*
 * list_get_count
 *
 * Return the countof elements in the given list
 */
uint32_t
list_get_count (list_t *list)
{
    return list->list_count;
}

/*
 * list_empty
 *
 * Returns true if list is empty. False otherwise
 */
uint8_t
list_empty (list_t *list)
{
    if (list->list_count == 0) {
        return TRUE;
    }

    return FALSE;
}

/*
 * list_insert
 *
 * Insert an element into the tail of the list
 */
int
list_insert (list_t *list, list_elem_t *elem)
{
    list_elem_t *link;

    /* Sanity check */
    if (!list || !elem) {
        return EINVAL;
    }

    if (list_empty(list)) {
        /* First element */
        list->list_head = elem;
        list->list_tail = elem;
        elem->next = NULL;
    } else {
        /* Insert it in the end */
        link = list->list_tail;
        elem->next = NULL;
        link->next = elem;
        list->list_tail = elem;
    }

    /* Bump up the count */
    list->list_count++;

    return EOK;
}

/*
 * list_insert_head
 *
 * Insert an element into the head of the list
 */
int
list_insert_head (list_t *list, list_elem_t *elem)
{
    /* Sanity check */
    if (!list || !elem) {
        return EINVAL;
    }

    elem->next = list->list_head;
    list->list_head = elem;

    /* Bump up the count */
    list->list_count++;

    return EOK;
}

/*
 * list_insert_tail
 *
 * Insert an element into the tail of the list
 */
int
list_insert_tail (list_t *list, list_elem_t *elem)
{
    /* Just call list_insert */
    return (list_insert(list, elem));
}

/*
 * list_insert_after
 *
 * Insert an element after the given element
 */
int 
list_insert_after (list_t *list, list_elem_t *prev_elem, list_elem_t *elem)
{
    list_elem_t *link;

    /* Sanity check */
    if (!list || !prev_elem || !elem) {
        return EINVAL;
    }

    link = list->list_head;
    while (link != NULL) {
        if (link == prev_elem) {
            elem->next = prev_elem->next;
            prev_elem->next = elem;

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
 * list_insert_before
 *
 * Insert an element before the given element
 */
int
list_insert_before (list_t *list, list_elem_t *next_elem, list_elem_t *elem)
{
    list_elem_t *link;

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
            link->next = elem;

            /* Bump up the count */
            list->list_count++;

            return EOK;
        }

        link = link->next;
    }

    return ENOTFOUND;
}

/*
 * list_remove
 *
 * Remove an element from the list
 */
int
list_remove (list_t *list, list_elem_t *elem)
{
    list_elem_t *link;

    /* Sanity check */
    if (!list || !elem) {
        return EINVAL;
    }

    link = list->list_head;

    /* Special case: Are we asked to remove the head? */
    if (link == elem) {
        list->list_head = link->next;
        list->list_count--;
        return EOK;
    }

    while (link != NULL) {
        if (link->next == elem) {
            link->next = elem->next;

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
 * list_default_cmp_fn
 *
 * Compares two list elements by examining their respective keys.
 * Note that this function just compares the pointers associated
 * with the keys. If the specific key values need to be compared,
 * then the application specific compare function has to be passed.
 */
static int32_t
list_default_cmp_fn (void *key1, void *key2)
{
    int32_t result = (uint32_t)key1 - (uint32_t)key2;

    return result;
}

/*
 * list_find
 *
 * This routine tries to lookup an element in the given key using the
 * supplied key and a compare function. If no compare function is
 * passed, the default compare function is used.
 */
void *
list_find (list_t *list, void *key, int32_t (*cmp_fn)(void *key1, void *key2))
{
    list_elem_t *elem;
    void *elem_key;

    /* Sanity check */
    if (!list || !key) {
        return NULL;
    }

    /* Use the default compare function ifnothing is passed */
    if (!cmp_fn) {
        cmp_fn = list_default_cmp_fn;
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
