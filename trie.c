/*
 * trie.c - This file contains a trie implementation where each object
 *          is keyed by a string
 */

/*
 * Sample representation of the Trie datastructure
 * 
 * Each trie node contains a sibling pointer, child pointer and a
 * parent pointer. The set containing keys ann, andrew, annabel
 * and dilbert will be represented as:
 * 
 *       a ---------------> d
 *       n                  i
 *       n--------> d       l
 *       0--> a     r       b
 *            b     e       e
 *            e     w       r
 *            l     0       t
 *            0             0
 *          
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trie.h"

/*
 * trie_create
 *
 * Create an instance of the trie and return a pointer to it
 */
trie_t *
trie_create (char *name, char* (*get_key)(void *trie_node))
{
    trie_t  *trie;

    trie = (trie_t *)malloc(sizeof(trie_t));
    if (!trie) {
        return NULL;
    }

    /* Initialize the contents */
    strncpy(trie->trie_name, name, strlen(name));
    trie->node_count = 0;
    trie->root = NULL;
    trie->get_key = get_key;

    return trie;
}

/*
 * trie_destroy
 *
 * Free the given trie
 */
int
trie_destroy (trie_t *trie)
{
    /* Bail if the trie is not empty */
    if (!trie_empty(trie)) {
        return EFAIL;
    }

    /* Do the deed */
    free(trie);

    return EOK;
}

/*
 * trie_get_least_internal
 *
 * Return the leaf node from the given level
 */
static trie_node_t *
trie_get_least_internal (trie_node_t *level)
{
    /* Go down the trie children by children till we hit a leaf */
    while (level != NULL) {
        if (level->key == 0) {
            return level->data;
        }
        level = level->children;
    }

    /* Shouldn't come here */
    return NULL;
}

/*
 * trie_get_least
 *
 * Return a pointer to the first key in the trie
 */
void *
trie_get_least (trie_t *trie)
{
    trie_node_t *root = trie->root;
    void *data;

    /* Sanity check */
    if (!root) {
        return NULL;
    }

    /* Go down the trie one level at a time till we hit a leaf */
    data = trie_get_least_internal(root);

    /* Return whatever we got */
    return data;
}

/*
 * trie_get_next
 *
 * Return the next key in the trie
 */
void *
trie_get_next (trie_t *trie, void *prev_node)
{
    trie_node_t *level, *key_node, *node, *parent;
    char *key;

    /* Sanity check */
    if (!trie || !prev_node || !trie->root) {
        return NULL;
    }

    /* Get the key associated with the previous node */
    key = trie->get_key(prev_node);
    if (!key) {
        return NULL;
    }

    /* First get the leaf corresponding to the given key */
    key_node = (trie_node_t *)trie_lookup_internal(trie, key);
    if (!key_node) {
        return NULL;
    }

    /* 
     * Walk up the trie one level at a time. Stop when you
     * reach a level which is having a right sibling.
     */
    parent = key_node;
    node = NULL;

    while (parent != NULL) {
        if (parent->sibling != NULL) {
            if (parent->sibling != node) {
                break;
            }
        }
        node = parent;
        parent = node->parent;
    }

    /* Bail if we didn't find anything */
    if (!parent) {
        /* We have iterated over all the nodes */
        return NULL;
    }

    /* 
     * We have come to the correct branch. Pick the leaf node
     * for this branch and return the corresponding data.
     */
    return (trie_get_least_internal(parent->sibling));
}

/*
 * trie_get_count
 *
 * Return the count of elements in the given trie
 */
uint32_t
trie_get_count (trie_t *trie)
{
    return trie->leaf_count;
}

/*
 * trie_empty
 *
 * Returns true if trie is empty. False otherwise.
 */
uint8_t
trie_empty (trie_t *trie)
{
    if (trie->node_count == 0) {
        return TRUE;
    }

    return FALSE;
}

/*
 * trie_insert_internal
 *
 * This routine is called from trie_insert() once a branching point is
 * reached. From the branching point, we just have to create trie nodes
 * for each of the remaining elements of the key and store the value
 * in the leaf node.
 */
static int
trie_insert_internal (trie_t *trie, trie_node_t *parent, 
                      char *key, void *data, int key_index) 
{
    trie_node_t *new_node;

    /*
     * Start from level = parent and keep adding internal nodes till we
     * exhaust all characters in the key
     */
    while (1) {
        
        new_node = (trie_node_t *)malloc(sizeof(trie_node_t));
        if (!new_node) {
            return EFAIL;
        }

        if (key_index >= strlen(key)) {
            /*
             * We have come to the leaf node after creating trie nodes
             * for each character in the key. This is where we store the
             * actual value.
             */
            new_node->key = 0;
            new_node->data = data;
            new_node->sibling = NULL;
            new_node->children = NULL;
            new_node->parent = parent;
            parent->children = new_node;
            trie->node_count++;
            trie->leaf_count++;
            break;
        }

        /* Internal trie nodes holding the key */
        new_node->key = key[key_index];
        new_node->data = NULL;
        new_node->sibling = NULL;
        new_node->children = NULL;
        new_node->parent = parent;
        parent->children = new_node;
        parent = new_node;
        trie->node_count++;
        key_index++;
    }
}

/*
 * trie_insert
 *
 * Insert a node to the trie
 */
int
trie_insert (trie_t *trie, char *key, void *data)
{
    trie_node_t *root, *level, *node, *prev_node, *new_node, *last_node;
    int         key_index;
    uint8_t     match_found;

    /* Sanity check */
    if (!trie || !key) {
        return EINVAL;
    }

    /* Grab a pointer to the root */
    root = trie->root;

    /* Check if this is the first element in the trie */
    if (!root) {
        root = (trie_node_t *)malloc(sizeof(trie_node_t));
        trie->node_count++;
        root->key = key[0];
        root->data = NULL;
        root->sibling = NULL;
        root->children = NULL;
        root->parent = NULL;
        trie->root = root;

        if (key[0] == 0) {
            /* We are done */
            return;
        }

        /* Scan through each character of the key and create nodes for them */
        return (trie_insert_internal(trie, root, key, data, 1));
    }
        
    /*
     * Check if there is a common prefix already present. If not, we 
     * have to create a new one.
     */
    level = root;

    for (key_index = 0; key_index < strlen(key);) {

        /* Search all nodes in the current level for a match */
        node = level;
        match_found = FALSE;

        while (node != NULL) {
            prev_node = node;
            if (node->key == key[key_index]) {
                match_found = TRUE;
                break;
            }
            node = node->sibling;
        }

        /* Did we find a match? */
        if (match_found == TRUE) {
            /* Jump to the next level */
            level = node->children;;
            key_index++;
        } else {
            /* 
             * We have come to a branching point. Create a new set of 
             * nodes for the remaining characters in the key
             */
            new_node = (trie_node_t *)malloc(sizeof(trie_node_t));
            trie->node_count++;

            new_node->key = key[key_index];
            new_node->data = NULL;
            new_node->sibling = NULL;
            new_node->children = NULL;
            new_node->parent = prev_node;
            prev_node->sibling = new_node;
            key_index++;

            return (trie_insert_internal(trie, new_node, key, data, key_index));
        }
    }

    /* 
     * If we come here, then we were passed a key which is a prefix of
     * an already existing longer key. First check if we were passed
     * a duplicate key.
     */
    if (level->key == 0) {
        printf("Duplicate key passed: %s\n", key);
        return EFAIL;
    }

    /* 
     * Create the leaf node for this key. Other nodes have already been
     * created.
     */
    last_node = (trie_node_t *)malloc(sizeof(trie_node_t));
    trie->node_count++;
    trie->leaf_count++;
    last_node->key = 0;
    last_node->data = data;
    last_node->sibling = NULL;
    last_node->children = NULL;
    last_node->parent = level;
    level->sibling = last_node;

    return EOK;
}

/*
 * trie_remove
 *
 * Remove a node from the trie
 */
int
trie_remove (trie_t *trie, char *key)
{
    trie_node_t     *root, *level, *node, *parent;
    trie_del_node_t delete_arr[MAX_KEY_LEN];
    int             key_index;
    uint8_t         match_found;

    /* Sanity check */
    if (!trie || !key) {
        return EINVAL;
    }

    /* Grab a pointer to the root */
    root = trie->root;
    if (!root) {
        return EINVAL;
    }

    level = root;
    parent = NULL;

    /*
     * Populate the temporary delete array. The purpose of using this
     * array is to simplify the process of identifying what all nodes
     * have to be removed as many keys can share common prefixes. For
     * each element in the key, the delete array contains:
     * - The node to be deleted
     * - The first node in the linked list at the current level
     * - The parent node
     * - Number of siblings
     * - Number of children
     */
    for (key_index = 0; key_index < strlen(key); key_index++) {

        node = level;
        match_found = FALSE;
        delete_arr[key_index].first_node = node;
        delete_arr[key_index].num_siblings = 0;
        delete_arr[key_index].num_children = 0;

        while (node != NULL) {
            if (node->key == key[key_index]) {
                match_found = TRUE;
                delete_arr[key_index].del_node = node;
                delete_arr[key_index].parent = parent;
                parent = node;
                level = node->children;
            }
            delete_arr[key_index].num_siblings++;
            node = node->sibling;
        }

        if (key_index > 0) {
            delete_arr[key_index - 1].num_children = delete_arr[key_index].num_siblings;
        }

        if (match_found == FALSE) {
            return ENOTFOUND;
        }
    }

    /* 
     * An additional iteration to cover the leaf node. We need this to
     * handle cases like deleting "ann" in a trie containing "ann" and
     * "annabel" as keys 
     */
    node = level;
    match_found = FALSE;;
    delete_arr[key_index].first_node = node;
    delete_arr[key_index].num_siblings = 0;
    delete_arr[key_index].num_children = 0;

    while (node != NULL) {
        if (node->key == 0) {
            match_found = TRUE;
            delete_arr[key_index].del_node = node;
            delete_arr[key_index].parent = parent;
        }
        delete_arr[key_index].num_siblings++;
        node = node->sibling;
    }

    if (key_index > 0) {
        delete_arr[key_index - 1].num_children = delete_arr[key_index].num_siblings;
    }

    if (match_found == FALSE) {
        return ENOTFOUND;
    }

    /*
     * Now that we have the delete array, our job is almost done. We start
     * from the last node. Traverse up one node at a time till we come to a
     * branching point. This means, the parent has more than one child. Here
     * we stop and adjust the pointers. Remember to free the nodes.
     */
    for (key_index = strlen(key); key_index >= 0; key_index--) {

        if (delete_arr[key_index - 1].num_children == 1) {
            /* Can remove this node as parent has only this one child */
            free(delete_arr[key_index].del_node);
            trie->node_count--;
            continue;
        }

        /* 
         * We have come to a branching point. We need to adjust the parent
         * and sibling pointers here.
         */
        if (delete_arr[key_index].first_node == delete_arr[key_index].del_node) {
            /* 
             * Special case. Node to be deleted is the first node in the 
             * list. Set the parent to point to the next sibling. If we
             * are already at the top level, then we are blowing away
             * trie->root. Set the trie->root pointer to point to the
             * next sibling.
             */
            if (key_index == 0) {
                trie->root = delete_arr[key_index].del_node->sibling;
            } else {
                delete_arr[key_index].parent->children = 
                    delete_arr[key_index].first_node->sibling;
            }
            free(delete_arr[key_index].del_node);
            trie->node_count--;
        } else {
            node = delete_arr[key_index].first_node;

            /* Traverse till we reach the victim node and then adjust the pointer */
            while (node->sibling != delete_arr[key_index].del_node) {
                node = node->sibling;
            }

            /*
             * If we are at the top level, we need to adjust the parent
             * pointer of the deleted node's sibling
             */
            if (key_index == 0 && node->sibling->sibling) {
                node->sibling->sibling->parent = node;
            }

            node->sibling = node->sibling->sibling;
            free(delete_arr[key_index].del_node);
            trie->node_count--;
        }

        /* All done */
        break;
    }

    trie->leaf_count--;

    return EOK;
}

/*
 * trie_lookup_internal
 *
 * Same as trie_lookup but returns a pointer to the leaf trie node
 * instead of the actual data. Used by trie_get_next()
 */
void *
trie_lookup_internal (trie_t *trie, char *key)
{
    trie_node_t  *root, *level, *node;
    uint8_t match_found = FALSE;
    int i = 0;

    /* Sanity check */
    if (!trie) {
        return NULL;
    }

    /* Grab a pointer to the root */
    root = trie->root;
    if (!root) {
        return NULL;
    }

    /* Search level by level */
    level = root;
    while (level != NULL) {
        node = level;
        while (node != NULL) {
            if (node->key == key[i]) {
                level = node->children;
                match_found = TRUE;

                /* Are we at the leaf node? */
                if (key[i] == 0) {
                    /* Match found */
                    return node;
                }

                i++;
                break;
            }
            node = node->sibling;
        }

        /* Bail if nothing matched */
        if (match_found == FALSE) {
            return NULL;
        }
    }
}

/*
 * trie_lookup
 *
 * Lookup a node in the trie with the given key. Returns NULL
 * if node is not found.
 */
void *
trie_lookup (trie_t *trie, char *key)
{
    trie_node_t  *root, *level, *node;
    uint8_t match_found = FALSE;
    int i = 0;

    /* Sanity check */
    if (!trie) {
        return NULL;
    }

    /* Grab a pointer to the root */
    root = trie->root;
    if (!root) {
        return NULL;
    }

    /* Search level by level */
    level = root;
    while (level != NULL) {
        node = level;
        while (node != NULL) {
            if (node->key == key[i]) {
                level = node->children;
                match_found = TRUE;

                /* Are we at the leaf node? */
                if (key[i] == 0) {
                    /* Match found */
                    return node->data;
                }

                i++;
                break;
            }
            node = node->sibling;
        }

        /* Bail if nothing matched */
        if (match_found == FALSE) {
            return NULL;
        }
    }
}

/* End of File */
