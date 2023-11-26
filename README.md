# storybox
(ALPHA) Helpful toolkit for developing text adventure games written in C. Single file, Header only. Easy inclusion.
```c
#include<storybox.h>
// ...
int main(int argc, char **argv) {
    __init_storybox_tools();
    // game on! (get it?)
    // side note, always be careful about the init function.
    // if it is ran after initialization, it will exit the program immediately.
    // same with the other storybox functions. they will immediately exit the program if ran without initialization.
}
```

## Features:
- Typing-style log to stdout with delay and of course, Press any key to skip.
- Multiple choice questions. Cycle between choices with the `space` key and pick one with the `enter` key.
- That's it for now, More features are coming up!