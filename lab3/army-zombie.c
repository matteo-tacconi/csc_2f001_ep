#include <stdio.h>
#include <stdlib.h>
#include "army.h"

struct node {
    struct monster* monster;
    struct node* next;
};

struct army {
    struct node* first;
    int stars;
};

struct army* create_army() {
    struct army* a = malloc(sizeof(struct army));
    if (a) {
        a->first = NULL;
        a->stars = 0;
    }
    return a;
}

void add_monster_to_army(struct army* a, struct monster* m) {
    struct node* new_node = malloc(sizeof(struct node));
    if (new_node) {
        new_node->monster = m;
        new_node->next = a->first;
        a->first = new_node;
    }
}

void display_army(struct army* a) {
    printf("This is a %d stars army\n", a->stars);
    struct node* current = a->first;
    while (current != NULL) {
        display_monster(current->monster);
        current = current->next;
    }
}

void delete_army(struct army* a) {
    if (a) {
        struct node* current = a->first;
        while (current != NULL) {
            struct node* next = current->next;
            free(current);
            current = next;
        }
        free(a);
    }
}