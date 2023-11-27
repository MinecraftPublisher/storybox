#include "storybox.h"

bool has_intro = false;

void bye() {
    clearscreen();
    usleep(500 * 1000);
    typewriter("Thanks for testing me.\n\n");
}

int main() {
    clearscreen();

    if (!has_intro) {
        __init_storybox_tools();
        typewriter(
            "(fast-forward by pressing any key)\n-- STORYBOX --\nA text-based game development "
            "toolkit.\nMove onto the next choice by pressing space.\nChoose the selected option by "
            "pressing enter.\nPress space once at the last choice to cycle back to the start.");
        has_intro = true;
    }

    string arr[] = { "Option 1", "Option 2", "Option 3" };
    printf("YOU CHOSE: {%s}\n", arr[ choose(3, arr) ]);
    typewriter("Want to go again?");
    chooseAndRun(2, createChoices(2, "Yes", main, "No", bye));

    return 0;
}
