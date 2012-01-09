/*
 * ds_usage.c - Sample program to demonstrate the usage of different data
 *              structure APIs
 */
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "list.h"
#include "llist.h"
#include "bst.h"
#include "trie.h"

/*
 * Example record for demonstrating usage of singly linked list APIs
 */
typedef struct employee_ {
    uint32_t    emp_id;
    uint32_t    emp_age;
    list_elem_t link;
} employee_t;

/*
 * Example record for demonstrating usage of doubly linked list APIs
 */
typedef struct student_ {
    uint32_t        stu_id;
    uint32_t        stu_age;
    llist_elem_t    link;
} student_t;

/*
 * Example record for demonstrating usage of binary search tree APIs
 */
typedef struct object_ {
    uint32_t        obj_id;
    uint32_t        obj_size;
    bst_node_t      bst_node;
} object_t;

/*
 * Example record for demonstrating usage of trie APIs
 */
typedef struct professor_ {
    char            prof_name[MAX_NAME_LEN];
    uint32_t        prof_dept_id;
    uint32_t        prof_experience;
} professor_t;

/*
 * list_compare_fn
 *
 * Compare function required for list_find()
 */
int32_t
list_compare_fn (void *key, void *emp_elem)
{
    uint32_t emp_id = *(uint32_t *)key;
    employee_t *emp = (employee_t *)emp_elem;

    if (emp->emp_id == emp_id) {
        return 0;
    } else {
        return -1;
    }
}

/*
 * linked_list_usage
 *
 * Example code to demonstrate the usage of linked list APIs
 */
void
linked_list_usage (void)
{
    list_t *emp_list;
    employee_t emp_array[20];
    employee_t *emp, *emp_head, *emp_tail;
    uint32_t id;
    int i;

    /* Create the list */
    emp_list = list_create("Employee Details", offsetof(employee_t, link));

    /* Create the records */
    for (i = 0; i < 20; i++) {
        emp_array[i].emp_id = i;
        emp_array[i].emp_age = i * 10;
    }

    /* Insert the records into the list */
    for (i = 0; i < 10; i++) {
        list_insert(emp_list, &emp_array[i].link);
    }

    /* Print the records */
    emp = (employee_t *)list_get_head(emp_list);
    while (emp != NULL) {
        printf("ID: %d, Age: %d\n", emp->emp_id, emp->emp_age);
        emp = list_get_next(emp_list, emp);
    }
    printf("\n");

    /* Try inserting elements to the head and tail of the list */
    list_insert_head(emp_list, &emp_array[10].link);
    list_insert_tail(emp_list, &emp_array[11].link);

    /* Print the list again */
    emp = (employee_t *)list_get_head(emp_list);
    while (emp != NULL) {
        printf("ID: %d, Age: %d\n", emp->emp_id, emp->emp_age);
        emp = list_get_next(emp_list, emp);
    }
    printf("\n");

    /* Try inserting elements after and before a certain element */
    list_insert_after(emp_list, &emp_array[5].link, &emp_array[12].link);
    list_insert_before(emp_list, &emp_array[5].link, &emp_array[13].link);

    /* Print the list again */
    emp = (employee_t *)list_get_head(emp_list);
    while (emp != NULL) {
        printf("ID: %d, Age: %d\n", emp->emp_id, emp->emp_age);
        emp = list_get_next(emp_list, emp);
    }
    printf("\n");

    /* Get the head and tail elements of the list and print them */
    emp_head = list_get_head(emp_list);
    emp_tail = list_get_tail(emp_list);

    printf("Head: ID: %d, Age: %d\n", emp_head->emp_id, emp_head->emp_age);
    printf("Tail: ID: %d, Age: %d\n\n", emp_tail->emp_id, emp_tail->emp_age);

    /* Get the current list count and print it */
    printf("List Count: %d\n\n", list_get_count(emp_list));

    /* Remove few elements and check the count again */
    list_remove(emp_list, &emp_array[10].link);
    list_remove(emp_list, &emp_array[11].link);
    list_remove(emp_list, &emp_array[12].link);
    list_remove(emp_list, &emp_array[13].link);

    printf("List Count after deleting 4 elements: %d\n\n", list_get_count(emp_list));

    /* Print the list again */
    emp = (employee_t *)list_get_head(emp_list);
    while (emp != NULL) {
        printf("ID: %d, Age: %d\n", emp->emp_id, emp->emp_age);
        emp = list_get_next(emp_list, emp);
    }
    printf("\n");

    /* Print the head and tail elements again */
    emp_head = list_get_head(emp_list);
    emp_tail = list_get_tail(emp_list);

    printf("Head: ID: %d, Age: %d\n", emp_head->emp_id, emp_head->emp_age);
    printf("Tail: ID: %d, Age: %d\n\n", emp_tail->emp_id, emp_tail->emp_age);

    /* Test the list_find() API */
    emp = list_find(emp_list, &emp_array[5].emp_id, list_compare_fn);
    if (emp) {
        printf("Employee record found: ID: %d, Age: %d\n", 
               emp->emp_id, emp->emp_age);
    } else {
        printf("Employee record not found\n");
    }

    id = 100; /* Non existing record */
    emp = list_find(emp_list, &id, list_compare_fn);
    if (emp) {
        printf("Employee record found: ID: %d, Age: %d\n",
               emp->emp_id, emp->emp_age);
    } else {
        printf("Employee record not found\n");
    }
}

/*
 * llist_compare_fn
 *
 * Compare function required for llist_find()
 */
int32_t
llist_compare_fn (void *key, void *stu_elem)
{
    uint32_t stu_id = *(uint32_t *)key;
    student_t *stu = (student_t *)stu_elem;

    if (stu->stu_id == stu_id) {
        return 0;
    } else {
        return -1;
    }
}

/*
 * doubly_linked_list_usage
 *
 * Example code to demonstrate the usage of doubly linked list APIs
 */
void
doubly_linked_list_usage (void)
{
    llist_t *stu_list;
    student_t stu_array[20];
    student_t *stu, *stu_head, *stu_tail;
    uint32_t id;
    int i;

    /* Create the list */
    stu_list = llist_create("student Details", offsetof(student_t, link));

    /* Create the records */
    for (i = 0; i < 20; i++) {
        stu_array[i].stu_id = i;
        stu_array[i].stu_age = i * 10;
    }

    /* Insert the records into the list */
    for (i = 0; i < 10; i++) {
        llist_insert(stu_list, &stu_array[i].link);
    }

    /* Print the records */
    stu = (student_t *)llist_get_head(stu_list);
    while (stu != NULL) {
        printf("ID: %d, Age: %d\n", stu->stu_id, stu->stu_age);
        stu = llist_get_next(stu_list, stu);
    }
    printf("\n");

    /* Try inserting elements to the head and tail of the list */
    llist_insert_head(stu_list, &stu_array[10].link);
    llist_insert_tail(stu_list, &stu_array[11].link);

    /* Print the list again */
    stu = (student_t *)llist_get_head(stu_list);
    while (stu != NULL) {
        printf("ID: %d, Age: %d\n", stu->stu_id, stu->stu_age);
        stu = llist_get_next(stu_list, stu);
    }
    printf("\n");

    /* Try inserting elements after and before a certain element */
    llist_insert_after(stu_list, &stu_array[5].link, &stu_array[12].link);
    llist_insert_before(stu_list, &stu_array[5].link, &stu_array[13].link);

    /* Print the list again */
    stu = (student_t *)llist_get_head(stu_list);
    while (stu != NULL) {
        printf("ID: %d, Age: %d\n", stu->stu_id, stu->stu_age);
        stu = llist_get_next(stu_list, stu);
    }
    printf("\n");

    /* Get the head and tail elements of the list and print them */
    stu_head = llist_get_head(stu_list);
    stu_tail = llist_get_tail(stu_list);

    printf("Head: ID: %d, Age: %d\n", stu_head->stu_id, stu_head->stu_age);
    printf("Tail: ID: %d, Age: %d\n\n", stu_tail->stu_id, stu_tail->stu_age);

    /* Get the current list count and print it */
    printf("List Count: %d\n\n", llist_get_count(stu_list));

    /* Remove few elements and check the count again */
    llist_remove(stu_list, &stu_array[10].link);
    llist_remove(stu_list, &stu_array[11].link);
    llist_remove(stu_list, &stu_array[12].link);
    llist_remove(stu_list, &stu_array[13].link);

    printf("List Count after deleting 4 elements: %d\n\n", llist_get_count(stu_list));

    /* Print the list again */
    stu = (student_t *)llist_get_head(stu_list);
    while (stu != NULL) {
        printf("ID: %d, Age: %d\n", stu->stu_id, stu->stu_age);
        stu = llist_get_next(stu_list, stu);
    }
    printf("\n");

    /* Print the head and tail elements again */
    stu_head = llist_get_head(stu_list);
    stu_tail = llist_get_tail(stu_list);

    printf("Head: ID: %d, Age: %d\n", stu_head->stu_id, stu_head->stu_age);
    printf("Tail: ID: %d, Age: %d\n\n", stu_tail->stu_id, stu_tail->stu_age);

    /* Test the llist_find() API */
    stu = llist_find(stu_list, &stu_array[5].stu_id, llist_compare_fn);
    if (stu) {
        printf("student record found: ID: %d, Age: %d\n", 
               stu->stu_id, stu->stu_age);
    } else {
        printf("student record not found\n");
    }

    id = 100; /* Non existing record */
    stu = llist_find(stu_list, &id, llist_compare_fn);
    if (stu) {
        printf("student record found: ID: %d, Age: %d\n",
               stu->stu_id, stu->stu_age);
    } else {
        printf("student record not found\n");
    }
}

/*
 * bst_get_key
 *
 * Return the key for the given node. Called from the BST library.
 */
int
bst_get_key (void *node)
{
    object_t *obj = (object_t *)node;

    if (!obj) {
        return 0;
    }

    return obj->obj_id;
}

/*
 * bst_usage
 *
 * Example code to demonstrate the usage of BST APIs
 */
void
bst_usage (void)
{
    bst_t *obj_tree;
    object_t obj_array[20];
    object_t *obj;
    uint32_t id;
    int i;

    /* Create the BST */
    obj_tree = bst_create("Object Details", offsetof(object_t, bst_node),
                          bst_get_key);

    /* Create the nodes */
    obj_array[0].obj_id = 50;
    obj_array[0].obj_size = 500;
    obj_array[1].obj_id = 25;
    obj_array[1].obj_size = 250;
    obj_array[2].obj_id = 75;
    obj_array[2].obj_size = 750;
    obj_array[3].obj_id = 10;
    obj_array[3].obj_size = 100;
    obj_array[4].obj_id = 40;
    obj_array[4].obj_size = 400;
    obj_array[5].obj_id = 5;
    obj_array[5].obj_size = 50;
    obj_array[6].obj_id = 15;
    obj_array[6].obj_size = 150;
    obj_array[7].obj_id = 30;
    obj_array[7].obj_size = 300;
    obj_array[8].obj_id = 45;
    obj_array[8].obj_size = 450;
    obj_array[9].obj_id = 60;
    obj_array[9].obj_size = 600;
    obj_array[10].obj_id = 90;
    obj_array[10].obj_size = 900;
    obj_array[11].obj_id = 55;
    obj_array[11].obj_size = 550;
    obj_array[12].obj_id = 80;
    obj_array[12].obj_size = 800;
    obj_array[13].obj_id = 100;
    obj_array[13].obj_size = 100;
    obj_array[14].obj_id = 65;
    obj_array[14].obj_size = 650;

    /* Insert the records into the list */
    for (i = 0; i < 15; i++) {
        bst_insert(obj_tree, &obj_array[i].bst_node);
    }

    /* Print the records */
    obj = (object_t *)bst_get_least(obj_tree);
    while (obj != NULL) {
        printf("ID: %d, Size: %d\n", obj->obj_id, obj->obj_size);
        obj = bst_get_next(obj_tree, obj);
    }
    printf("\n");

    /* Get the current node count and print it */
    printf("Object Count: %d\n\n", bst_get_count(obj_tree));

    /* Remove few elements and check the count again */
    bst_remove(obj_tree, &obj_array[1].bst_node);
    bst_remove(obj_tree, &obj_array[5].bst_node);
    bst_remove(obj_tree, &obj_array[9].bst_node);
    bst_remove(obj_tree, &obj_array[13].bst_node);

    printf("Object Count after deleting 4 elements: %d\n\n", bst_get_count(obj_tree));

    /* Print the records again */
    obj = (object_t *)bst_get_least(obj_tree);
    while (obj != NULL) {
        printf("ID: %d, Size: %d\n", obj->obj_id, obj->obj_size);
        obj = bst_get_next(obj_tree, obj);
    }
    printf("\n");

    /* Test the bst_lookup() API */
    obj = bst_lookup(obj_tree, obj_array[10].obj_id);
    if (obj) {
        printf("Object record found: ID: %d, Size: %d\n", 
               obj->obj_id, obj->obj_size);
    } else {
        printf("Object record not found\n");
    }

    id = 100; /* Non existing record */
    obj = bst_lookup(obj_tree, id);
    if (obj) {
        printf("Object record found: ID: %d, Size: %d\n",
               obj->obj_id,obj->obj_size);
    } else {
        printf("Object record not found\n");
    }
}

/*
 * trie_get_key
 *
 * Return the key for the given node. Called from the Trie library.
 */
char *
trie_get_key (void *node)
{
    professor_t *prof = (professor_t *)node;

    if (!prof) {
        return 0;
    }

    return prof->prof_name;
}

/*
 * trie_usage
 *
 * Example code to demonstrate the usage of Trie APIs
 */
void
trie_usage (void)
{
    trie_t *prof_list;
    professor_t prof_array[10];
    professor_t *prof;
    char *invalid_key = "xxxxxyyyyyzzzzz";
    int i;

    /* Create the Trie */
    prof_list = trie_create("professor Details", trie_get_key);

    /* Create the nodes */
    strcpy(prof_array[0].prof_name, "ann");
    prof_array[0].prof_dept_id = 0;
    prof_array[0].prof_experience = 0;
    strcpy(prof_array[1].prof_name, "dileep");
    prof_array[1].prof_dept_id = 1;
    prof_array[1].prof_experience = 10;
    strcpy(prof_array[2].prof_name, "dilbert");
    prof_array[2].prof_dept_id = 2;
    prof_array[2].prof_experience = 20;
    strcpy(prof_array[3].prof_name, "annabel");
    prof_array[3].prof_dept_id = 3;
    prof_array[3].prof_experience = 30;
    strcpy(prof_array[4].prof_name, "andrew");
    prof_array[4].prof_dept_id = 4;
    prof_array[4].prof_experience = 40;
    strcpy(prof_array[5].prof_name, "deepak");
    prof_array[5].prof_dept_id = 5;
    prof_array[5].prof_experience = 50;
    strcpy(prof_array[6].prof_name, "andy");
    prof_array[6].prof_dept_id = 6;
    prof_array[6].prof_experience = 60;
    strcpy(prof_array[7].prof_name, "bill");
    prof_array[7].prof_dept_id = 7;
    prof_array[7].prof_experience = 70;
    strcpy(prof_array[8].prof_name, "billy");
    prof_array[8].prof_dept_id = 8;
    prof_array[8].prof_experience = 80;
    strcpy(prof_array[9].prof_name, "bichel");
    prof_array[9].prof_dept_id = 9;
    prof_array[9].prof_experience = 90;

    /* Insert the records into the list */
    for (i = 0; i < 10; i++) {
        trie_insert(prof_list, prof_array[i].prof_name, &prof_array[i]);
    }

    /* Print the records */
    prof = (professor_t *)trie_get_least(prof_list);
    while (prof != NULL) {
        printf("Name: %s Dept: %d Experience: %d\n", 
               prof->prof_name, prof->prof_dept_id, prof->prof_experience);
        prof = trie_get_next(prof_list, prof);
    }
    printf("\n");

    /* Get the current node count and print it */
    printf("Professor Count: %d\n\n", trie_get_count(prof_list));

    /* Remove few elements and check the count again */
    trie_remove(prof_list, prof_array[1].prof_name);
    trie_remove(prof_list, prof_array[3].prof_name);
    trie_remove(prof_list, prof_array[5].prof_name);
    trie_remove(prof_list, prof_array[7].prof_name);

    printf("Professor Count after deleting 4 elements: %d\n\n", trie_get_count(prof_list));

    /* Print the records again */
    prof = (professor_t *)trie_get_least(prof_list);
    if (!prof) {
        printf("Trie is empty!\n");
    }

    while (prof != NULL) {
        printf("Name: %s Dept: %d Experience: %d\n", 
               prof->prof_name, prof->prof_dept_id, prof->prof_experience);
        prof = trie_get_next(prof_list, prof);
    }
    printf("\n");

    /* Test the trie_lookup() API */
    prof = trie_lookup(prof_list, prof_array[6].prof_name);
    if (prof) {
        printf("Professor record found: Name: %s Dept: %d, Experience: %d\n",
               prof->prof_name, prof->prof_dept_id, prof->prof_experience);
    } else {
        printf("Professor record not found\n");
    }

    /* Non existing record */
    prof = trie_lookup(prof_list, invalid_key);
    if (prof) {
        printf("Professor record found: Name: %s Dept: %d, Experience: %d\n",
               prof->prof_name, prof->prof_dept_id, prof->prof_experience);
    } else {
        printf("Professor record not found\n");
    }
}

/* Main entry point */
int 
main (int argc, char *argv[])
{
    /* Linked list APIs */
    linked_list_usage();

    /* Doubly linked list APIs */
    doubly_linked_list_usage();

    /* Binary search tree APIs */
    bst_usage();
    
    /* Trie APIs */
    trie_usage();

    return 0;
}

/* End of File */
