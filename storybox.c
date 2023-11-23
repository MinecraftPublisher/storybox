#include "toolbox.h"

int main() {
    __init_storybox_tools();

    typewriter("-- STORYBOX --\nA text-based game development toolkit.");
    string arr[] = { "Option 1", "Option 2", "Option 3" };
    printf("YOU CHOSE: {%s}\n", arr[choose(3, arr)]);

    return 0;
}
