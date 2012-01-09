#ifndef TRIE_H
#define TRIE_H

#include <stdint.h>

/* Defines */

#define MAX_NAME_LEN                64
#define MAX_KEY_LEN                 64

#define TRUE                         1
#define FALSE                        0

#define EOK                          0
#define EINVAL                      -1
#define ENOTFOUND                   -2
#define EFAIL                       -3

/* Structure Definitions */

typedef struct trie_node_ {
    char                key;
    void                *data;
    struct trie_node_   *sibling;
    struct trie_node_   *children;
    struct trie_node_   *parent;
} trie_node_t;

typedef struct trie_del_node_ {
    trie_node_t     *del_node;
    trie_node_t     *first_node;
    trie_node_t     *parent;
    uint32_t        num_siblings;
    uint32_t        num_children;
} trie_del_node_t;

typedef struct trie_ {
    char            trie_name[MAX_NAME_LEN];
    trie_node_t     *root;
    uint32_t        node_count; /* Includes internal nodes as well. For debugging */
    uint32_t        leaf_count; /* This contains the actual number of records */
    char*           (*get_key)(void *trie_node);
} trie_t;

/* Function prototypes */

trie_t* trie_create (char *name, char* (*get_key)(void *trie_node));
int trie_destroy (trie_t *trie);
void* trie_get_root (trie_t *trie);
void* trie_get_least (trie_t *trie);
void* trie_get_next (trie_t *trie, void *prev_node);
uint32_t trie_get_count (trie_t *trie);
uint8_t trie_empty (trie_t *trie);
int trie_insert (trie_t *trie, char *key, void *data);
int trie_remove (trie_t *trie, char *key);
void* trie_lookup_internal (trie_t *trie, char *key);
void* trie_lookup (trie_t *trie, char *key);

#endif /* TRIE_H */
