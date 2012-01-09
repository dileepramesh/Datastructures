/*
 * bst.c - This file contains a binary search tree implementation
 *         where each object has a 32-bit key
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bst.h"

/*
 * bst_create
 *
 * Create an instance of binary search tree and return a pointer to it
 */
bst_t *
bst_create (char *name, uint32_t offset, int (*get_key)(void *node))
{
    bst_t   *bst;

    bst = (bst_t *)malloc(sizeof(bst_t));
    if (!bst) {
        return NULL;
    }

    /* Initialize the contents */
    strncpy(bst->bst_name, name, strlen(name));
    bst->node_offset = offset;
    bst->node_count = 0;
    bst->root = NULL;
    bst->get_key = get_key;

    return bst;
}

/*
 * bst_destroy
 *
 * Free the given binary search tree instance
 */
int
bst_destroy (bst_t *bst)
{
    /* Bail if the tree is not empty */
    if (!bst_empty(bst)) {
        return EFAIL;
    }

    /* Do the deed */
    free(bst);

    return EOK;
}

/*
 * bst_find_min
 *
 * Find the node with the minimum key from the given level. The
 * delete flag is set to TRUE when its called from bst_remove().
 * If this flag is set, we have to additionally adjust the left
 * and right pointers for the least node's parent.
 */
static bst_node_t *
bst_find_min (bst_node_t *root, uint8_t delete)
{
    bst_node_t *node = root;
    bst_node_t *min_node;

    /* Sanity check */
    if (!root) {
        return NULL;
    }

    /* 
     * Special case: Check if the left subtree is null. If it is,
     * the given root is the minimum element.
     */
    if (node->left == NULL) {
        min_node = node;

        if (delete) {
            node->parent->right = NULL;
        }

        return min_node;
    }

    /* Walk down the left subtree till we get the last node */
    while (node->left != NULL && node->left->left != NULL) {
        node = node->left;
    }

    min_node = node->left;

    /*
     * If this is called from bst_remove_internal(), we have to
     * fix the child pointer for the minimum node's parent
     */
    if (delete) {
        node->left = NULL;
    }

    return min_node;
}

/*
 * bst_get_root
 *
 * Return a pointer to the root
 */
void *
bst_get_root (bst_t *bst)
{
    void *root = (uint8_t *)bst->root - bst->node_offset;

    return root;
}

/*
 * bst_get_least
 *
 * Return a pointer to the least valued node in the BST
 */
void *
bst_get_least (bst_t *bst)
{
    bst_node_t *root = bst->root;
    bst_node_t *node;

    /* Sanity check */
    if (!root) {
        return NULL;
    }

    node = bst_find_min(root, FALSE);
    if (!node) {
        return NULL;
    }

    return ((void *)((uint8_t *)node - bst->node_offset));
}

/*
 * bst_get_next
 *
 * Return the next node in the tree
 */
void *
bst_get_next (bst_t *bst, void *prev_node)
{
    bst_node_t  *node, *next_node, *parent;

    /* Sanity check */
    if (!bst || !prev_node) {
        return NULL;
    }

    /* Get back the pointer to the BST node */
    node = (bst_node_t *)((uint8_t *)prev_node + bst->node_offset);

    /*
     * We have 2 cases here:
     *
     * 1. Right subtree of the node is non-null. In this case, the next
     *    node is the smallest element in the right subtree
     * 2. Right subtree of the node is null. In this case, we have to
     *    move up the tree to find the first ancestor whose left child
     *    is also anancestor to the given node.
     */
    if (node->right != NULL) {
        next_node = bst_find_min(node->right, FALSE);
    } else {
        parent = node->parent;
        while ((parent != NULL) && (node == parent->right)) {
            node = parent;
            parent = node->parent;
        }

        next_node = parent;
    }

    /* Did we find something? */
    if (!next_node) {
        return NULL;
    }

    return ((void *)((uint8_t *)next_node - bst->node_offset));
}

/*
 * bst_get_count
 *
 * Return the count of elements in the given tree
 */
uint32_t
bst_get_count (bst_t *bst)
{
    return bst->node_count;
}

/*
 * bst_empty
 *
 * Returns true if tree is empty. False otherwise.
 */
uint8_t
bst_empty (bst_t *bst)
{
    if (bst->node_count == 0) {
        return TRUE;
    }

    return FALSE;
}

/*
 * bst_insert_internal
 *
 * This routine takes a root pointer and a key pointer and
 * recursively computes where the key node has to be inserted
 */
static bst_node_t *
bst_insert_internal (bst_t *bst, bst_node_t *root, bst_node_t *key_node)
{
    int root_key, key;

    if (root == NULL) {
        return key_node;
    }

    root_key = bst->get_key((uint8_t *)root - bst->node_offset);
    key = bst->get_key((uint8_t *)key_node - bst->node_offset);

    if (key < root_key) {
        root->left = bst_insert_internal(bst, root->left, key_node);
        root->left->parent = root;
    } else if (key > root_key) {
        root->right = bst_insert_internal(bst, root->right, key_node);
        root->right->parent = root;
    }

    return root;
}

/*
 * bst_insert
 *
 * Insert a node to the binary search tree
 */
int
bst_insert (bst_t *bst, bst_node_t *node)
{
    bst_node_t  *root;

    /* Sanity check */
    if (!bst || !node) {
        return EINVAL;
    }

    /* Grab a pointer to the root */
    root = bst->root;

    /* Initialize the left, right and parent pointers */
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;

    /* Special case. Is this the first element? */
    if (!root) {
        bst->root = node;
        bst->node_count++;
        return EOK;
    }

    /* Recursively compute where this node has to be inserted */
    bst_insert_internal(bst, root, node);

    /* Increment the node count */
    bst->node_count++;

    return EOK;
}

/*
 * bst_remove_internal
 *
 * This routine takes a root pointer and a key pointer and
 * recursively computes where the key node is located. It
 * then removes the key node and replaces it with the smallest
 * element in its right subtree.
 */
static bst_node_t *
bst_remove_internal (bst_t *bst, bst_node_t *root, bst_node_t *key_node)
{
    int root_key, key;
    bst_node_t *node;

    if (root == NULL) {
        /* Element not found */
        return NULL;
    }

    root_key = bst->get_key((uint8_t *)root - bst->node_offset);
    key = bst->get_key((uint8_t *)key_node - bst->node_offset);

    if (key < root_key) {
        /* Search the left subtree */
        root->left = bst_remove_internal(bst, root->left, key_node);
        if (root->left) {
            root->left->parent = root;
        }
    } else if (key > root_key) {
        /* Search the right subtree */
        root->right = bst_remove_internal(bst, root->right, key_node);
        if (root->right) {
            root->right->parent = root;
        }
    } else {
        /* Element found. First check if any subtree is NULL */
        if (!(root->left && root->right)) {
            if (root->left == NULL) {
                node = root->right;
            } else {
                node = root->left;
            }
            return node;
        }

        /* Replace that element with the smallest element in its right subtree */
        node = bst_find_min(root->right, TRUE);
        node->left = root->left;
        node->right = root->right;
        node->parent = root->parent;
        if (root->left) {
            root->left->parent = node;
        }
        if (root->right) {
            root->right->parent = node;
        }
        return node;
    }

    return root;
}

/*
 * bst_remove
 *
 * Remove a node from the binary search tree
 */
int
bst_remove (bst_t *bst, bst_node_t *node)
{
    bst_node_t  *root, *ret;

    /* Sanity check */
    if (!bst || !node) {
        return EINVAL;
    }

    /* Grab a pointer to the root */
    root = bst->root;
    if (!root) {
        return EINVAL;
    }

    /* Recursively find the position of the node and remove it */
    ret = bst_remove_internal(bst, root, node);
    if (!ret) {
        /* Key was not found */
        return ENOTFOUND;
    }

    /* Decrement the node count */
    bst->node_count--;

    return EOK;
}

/*
 * bst_lookup_internal
 *
 * Recursively search for the given node in the BST. Returns NULL if
 * node is not found.
 */
static bst_node_t *
bst_lookup_internal (bst_t *bst, bst_node_t *root, int key)
{
    int root_key;
    
    if (!root) {
        return NULL;
    }

    /* Get the key value associated with the root */
    root_key = bst->get_key((uint8_t *)root - bst->node_offset);

    if (key < root_key) {
        /* Search the left subtree */
        return (bst_lookup_internal(bst, root->left, key));
    } else if (key > root_key) {
        /* Search the right subtree */
        return (bst_lookup_internal(bst, root->right, key));
    } else {
        /* Match found */
        return root;
    }
}

/*
 * bst_lookup
 *
 * Lookup a node in the BST with the given key. Returns NULL
 * if node is not found.
 */
void *
bst_lookup (bst_t *bst, int key)
{
    bst_node_t  *root;
    bst_node_t  *key_node;

    /* Sanity check */
    if (!bst) {
        return NULL;
    }

    /* Grab a pointer to the root */
    root = bst->root;
    if (!root) {
        return NULL;
    }

    /* Recursively search for the node with the given key */
    key_node = bst_lookup_internal(bst, root, key);
    if (!key_node) {
        return NULL;
    }

    return ((void *)((uint8_t *)key_node - bst->node_offset));
}

/* End of File */
