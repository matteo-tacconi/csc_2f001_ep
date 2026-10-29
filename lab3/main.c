#include <stdio.h>
#include "macro.h"
#include "monster.h"
#include "army.h"

int main() {
    struct army* my_army = create_army();
    struct monster* monsters[10];
    char name_buffer[NAME_SIZE];

    for (int i = 0; i < 10; i++) {
        snprintf(name_buffer, NAME_SIZE, "Pikachu-%d", i);
        monsters[i] = create_monster(name_buffer, 100);
        add_monster_to_army(my_army, monsters[i]);
    }

    display_army(my_army);

    delete_army(my_army);
    for (int i = 0; i < 10; i++) {
        delete_monster(monsters[i]);
    }

    return 0;
}