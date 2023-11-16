#pragma once

// #include <conio.h>
#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

typedef char* string;
#define false 0
#define true  1

#define print(x)                                                                                   \
    ({                                                                                             \
        printf(x);                                                                                 \
        printf("\n")                                                                               \
    })

#define TIME    35  // time for each character
#define NEWLINE 800 // time for newline

#include <signal.h>
#include <stdio.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

struct termios old_tio, new_tio;

void isBackspaceKeyPressed();

int chars = 0;

void handleSignal(int signal) {
    if (signal == SIGUSR1 && chars == 0) chars = 10;
    isBackspaceKeyPressed();
}

void isBackspaceKeyPressed() {
    signal(SIGUSR1, handleSignal);
    signal(0, handleSignal);

    pid_t pid = fork();
    if (pid < 0) {
        // Fork failed.
        usleep(500000);
        isBackspaceKeyPressed();
    } else if (pid == 0) {
        // Child process.
        char c;
        read(STDIN_FILENO, &c, 1);
        kill(getppid(), SIGUSR1);
        _exit(0);
    }
}

#define dotsleep(x)                                                                                  \
    ({                                                                                             \
        if (chars == 0) usleep(x * 1000);                                                          \
        else if (chars == 1) {                                                                     \
            usleep(250 * 1000);                                                                    \
            chars--;                                                                               \
        } else                                                                                    \
            chars--;                                                                               \
    })

void typewriter(string x) {
    int i = 0;
    while (x[ i ] != '\0') {
        switch (x[ i ]) {
            case '\n':
                dotsleep(NEWLINE);
                printf("\n");
                fflush(stdout);
                dotsleep(NEWLINE / 2);
                break;
            default:
                dotsleep(TIME);
                printf("%c", x[ i ]);
                fflush(stdout);
                break;
        }

        i++;
    }

    dotsleep(NEWLINE);

    printf("\n");
    fflush(stdout);

    dotsleep(NEWLINE / 2);
}

void __init_storybox_tools() {
    tcgetattr(STDIN_FILENO, &old_tio);          // Get the current terminal settings.
    new_tio = old_tio;                          // Copy the current settings to new_tio.
    new_tio.c_lflag &= (~ICANON & ~ECHO);       // Disable canonical mode and echo.
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio); // Set the new settings.

    isBackspaceKeyPressed();
}
