#ifndef BST_H
#define BST_H

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

typedef struct bst_node_ {
    struct bst_node_    *left;
    struct bst_node_    *right;
    struct bst_node_    *parent;
} bst_node_t;

typedef struct bst_ {
    char            bst_name[MAX_NAME_LEN];
    bst_node_t      *root;
    uint32_t        node_offset;
    uint32_t        node_count;
    int             (*get_key)(void *node);
} bst_t;

/* Function prototypes */

bst_t* bst_create (char *name, uint32_t node_offset, 
                   int (*get_key)(void *node));
int bst_destroy (bst_t *bst);
void* bst_get_root (bst_t *bst);
void* bst_get_least (bst_t *bst);
void* bst_get_next (bst_t *bst, void *prev_node);
uint32_t bst_get_count (bst_t *bst);
uint8_t bst_empty (bst_t *bst);
int bst_insert (bst_t *bst, bst_node_t *node);
int bst_remove (bst_t *bst, bst_node_t *node);
void* bst_lookup (bst_t *bst, int key);

#endif /* BST_H */
