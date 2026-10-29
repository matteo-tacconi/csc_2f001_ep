#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "monster.h"

struct monster* create_monster(const char* name, int health) {
    struct monster* m = malloc(sizeof(struct monster));
    if (m) {
        strncpy(m->name, name, NAME_SIZE - 1);
        m->name[NAME_SIZE - 1] = '\0';
        m->health = health;
    }
    return m;
}

void display_monster(struct monster* m) {
    if (m) {
        printf("Monster: %s, Health: %d\n", m->name, m->health);
    }
}

void delete_monster(struct monster* m) {
    free(m);
}