#pragma once

#include <curses.h>
#include <fcntl.h>
#include <ncurses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <termios.h>
#include <unistd.h>

typedef char *string;
#define false 0
#define true  1

#define print(x)                                                                                   \
    ({                                                                                             \
        printf(x);                                                                                 \
        printf("\n")                                                                               \
    })

#define TIME    35  // time for each character
#define NEWLINE 800 // time for newline

struct termios old_tio, new_tio;

void isAnyKeyPressed();

int  KEY_PIPE[ 2 ];
bool PIPE_MADE = false;

int  chars      = 0;
char pressedKey = 0;

void handleSignal(int signal) {
    char *buff   = malloc(sizeof(char));
    int   status = read(KEY_PIPE[ 0 ], buff, 1);
    if (status != -1) pressedKey = *buff;

    fsync(KEY_PIPE[ 0 ]);

    if (chars == 0) chars = 10;

    isAnyKeyPressed();
}

typedef struct {
    string text;
    void (*callback)();
} choice;

void isAnyKeyPressed() {
    signal(SIGCHLD, handleSignal);

    if (!PIPE_MADE) {
        if (pipe(KEY_PIPE) < 0) exit(SIGIO);
        fcntl(KEY_PIPE[ 0 ], F_SETFL, O_NONBLOCK);
        fcntl(KEY_PIPE[ 1 ], F_SETFL, O_NONBLOCK);
        PIPE_MADE = true;
    }

    pid_t pid = fork();
    if (pid < 0) {
        // Fork failed.
        usleep(500000);
        isAnyKeyPressed();
    } else if (pid == 0) {
        // Child process.
        char *c;
        read(STDIN_FILENO, c, 1);

        // write data to pipe
        write(KEY_PIPE[ 1 ], c, 1);
        fsync(KEY_PIPE[ 1 ]);

        kill(getppid(), SIGCHLD);
        _exit(0);
    }
}

void dotsleep(int time) {
    if (chars == 0) usleep(time * 1000);
    else if (chars == 1) {
        usleep(100 * 1000);
        chars--;
    } else
        chars--;
}

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

#define createChoice(_text, cb)                                                                    \
    ({                                                                                             \
        choice c;                                                                                  \
        c.text     = _text;                                                                        \
        c.callback = cb;                                                                           \
        c;                                                                                         \
    })

choice *createChoices(int count, ...) {
    choice *result = malloc(sizeof(choice) * count);

    va_list list;
    va_start(list, count);

    for (int i = 0; i < count * 2; i += 2) {
        string text        = va_arg(list, string);
        void (*callback)() = va_arg(list, void (*)());

        result[ i / 2 ] = createChoice(text, callback);
    }
    va_end(list);

    return result;
}

void noncannon() {
    tcgetattr(STDIN_FILENO, &old_tio);          // Get the current terminal settings.
    new_tio = old_tio;                          // Copy the current settings to new_tio.
    new_tio.c_lflag &= (~ICANON & ~ECHO);       // Disable canonical mode and echo.
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio); // Set the new settings.
}

void cannon() {
    tcgetattr(STDIN_FILENO, &old_tio);          // Get the current terminal settings.
    new_tio = old_tio;                          // Copy the current settings to new_tio.
    new_tio.c_lflag &= (ICANON & ~ECHO);        // Enable canonical mode and echo.
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio); // Set the new settings.
}

int choose(int size, string *cs) {
    int choice = 0;

    int SIZE = 0;

    for (int i = 0; i < size; i++) {
        int x = printf("\n[%c] %s", i == choice ? 'x' : ' ', cs[ i ]);
        SIZE += x;

        fflush(stdout);
    }

    pressedKey = 0;
    while (pressedKey != '\n') {
        usleep(50000);

        if (pressedKey == ' ') {
            choice++;
            if (choice == size) choice = 0;
            for (int i = 0; i < SIZE; i++) {
                printf("\r");
                fflush(stdout);
            }
            printf("\033[%dA", size);

            for (int i = 0; i < size; i++) {
                int x = printf("\n[%c] %s", i == choice ? 'x' : ' ', cs[ i ]);

                fflush(stdout);
            }

            pressedKey = 0;
        }
    }

    printf("\n");

    return choice;
}

void chooseAndRun(int size, choice *cs) {
    string arr[ size ];
    for (int i = 0; i < size; i++) arr[ i ] = cs[ i ].text;
    int result = choose(size, arr);
    cs[ result ].callback();
}

void __init_storybox_tools() {
    // run the keypress detector daemon
    noncannon();
    isAnyKeyPressed();
}
