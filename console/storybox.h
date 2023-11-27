#pragma once

// ----------------------- LIBRARIES -----------------------
#include <fcntl.h>   // non-blocking pipe
#include <ncurses.h> // non-canonical mode and noecho
#include <signal.h>  // exit signal handling
#include <stdio.h>   // input/output
#include <stdlib.h>  // basic functions like exit etc
#include <termios.h> // needed alongside ncurses
#include <unistd.h>  // buffer manipulation
// ----------------------- LIBRARIES -----------------------

// ----------------------- FILE FUNCTIONS -----------------------

// types + bool
typedef uint8_t u8;
typedef char   *string;
#define false 0
#define true  1

typedef struct {
    u8     size;
    string content;
} File;

string DATA = "";
u8 DATA_SIZE = -1;

File *createFile(u8 size, string content) {
    File *newFile    = malloc(sizeof(File));
    newFile->content = content;
    newFile->size    = size;

    return newFile;
}

bool SILENT_FILE = false;

File *readFile(string fname) {
    FILE *fp = fopen(fname, "rb");
    if (!fp) {
        if(!SILENT_FILE) (void) fprintf(stderr, "Error opening file: %s.\n", fname);
        return createFile(0, NULL);
    }

    (void) fseek(fp, 0, SEEK_END);
    const int fsize = ftell(fp);

    (void) fseek(fp, 0, SEEK_SET);
    string b = (string)malloc(fsize);

    (void) fread(b, fsize, 1, fp);
    (void) fclose(fp);

    return createFile(fsize, b);
}

File *writeFile(string fname, u8 fsize, string content) {
    FILE *fp = fopen(fname, "wb");

    if (fp == NULL) {
        if(!SILENT_FILE) (void) fprintf(stderr, "Error opening file: %s.\n", fname);
        return createFile(0, NULL);
    }

    for(int i = 0; i < fsize; i++) {
        (void) fprintf(fp, "%c", content[i]);
    }
    (void) fclose(fp);

    return createFile(-1, content);
}

void loadData(string path) {
    SILENT_FILE = true;

    File *data = readFile(path);

    if(data->size != 0) {
        DATA = data->content;
        DATA_SIZE = data->size;
    }

    SILENT_FILE = false;
}
// ----------------------- FILE FUNCTIONS -----------------------

#pragma UNUSED
#define print(...)                                                                                 \
    ({                                                                                             \
        printf(__VA_ARGS__);                                                                       \
        printf("\n");                                                                               \
    })

#define TIME    35  // time for each character
#define NEWLINE 700 // time for newline

// termios states to recover and reload
struct termios old_tio, new_tio;

#pragma NOT_NEEDED
void isAnyKeyPressed();

int  KEY_PIPE[ 2 ];
bool PIPE_MADE = false;

bool HAS_INIT = false;

int  chars      = 0;
char pressedKey = 0;
int  repeat     = 0;

// handle keypress signal
void handleSignal(int signal) {
    if (!HAS_INIT) {
        print("CRITICLA ERROR: Running storybox function without initialization.");
        exit(SIGINFO);
    }

    char *buff   = (char *) malloc(sizeof(char));
    int   status = read(KEY_PIPE[ 0 ], buff, 1);
    if (status != -1) {
        if (pressedKey == *buff) repeat++;
        else
            repeat = 0;

        pressedKey = *buff;
    }

    fsync(KEY_PIPE[ 0 ]);

    if (chars == 0) chars = 10;
}

// choice struct
typedef struct {
    string text;
    void (*callback)();
} choice;

// keypress handler
void isAnyKeyPressed() {
    if (!HAS_INIT) {
        print("CRITICLA ERROR: Running storybox function without initialization.");
        exit(SIGINFO);
    }

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
        while (true) {
            char *c;
            read(STDIN_FILENO, c, 1);

            // write data to pipe
            write(KEY_PIPE[ 1 ], c, 1);
            fsync(KEY_PIPE[ 1 ]);
            kill(getppid(), SIGCHLD); // signal parent that it should read the pipe
            usleep(50000);
        }
    }
}

// sleep unless keypress was detected
void dotsleep(int time) {
    if (!HAS_INIT) {
        print("CRITICLA ERROR: Running storybox function without initialization.");
        exit(SIGINFO);
    }

    if (chars == 0) usleep(time * 1000);
    else if (chars == 1) {
        usleep(100 * 1000);
        chars--;
    } else
        chars--;
}

// wether or not to use a newline
bool typewriter_newline = true;

void typewriter(string x) {
    if (!HAS_INIT) {
        print("CRITICLA ERROR: Running storybox function without initialization.");
        exit(SIGINFO);
    }

    int i = 0;
    while (x[ i ] != '\0') {
        switch (x[ i ]) {
            case '\n':
                dotsleep(NEWLINE);
                print("");
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

    if (typewriter_newline) {
        dotsleep(NEWLINE);
        print("");
    }
    fflush(stdout);

    typewriter_newline = false;
    dotsleep(NEWLINE / 2);
}

// handy tool to create a choice object
#define createChoice(_text, cb)                                                                    \
    ({                                                                                             \
        choice c;                                                                                  \
        c.text     = _text;                                                                        \
        c.callback = cb;                                                                           \
        c;                                                                                         \
    })

// handier tool to create a bunch of choice objects at once
choice *createChoices(int count, ...) {
    if (!HAS_INIT) {
        print("CRITICLA ERROR: Running storybox function without initialization.");
        exit(SIGINFO);
    }

    choice *result = (choice *) malloc(sizeof(choice) * count);

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

int choose(int size, string *cs) {
    if (!HAS_INIT) {
        print("CRITICLA ERROR: Running storybox function without initialization.");
        exit(SIGINFO);
    }

    int choice = 0;

    int SIZE = 0;

    for (int i = 0; i < size; i++) {
        int x              = printf("\n[%c] ", i == choice ? 'x' : ' ');
        typewriter_newline = false;
        typewriter(cs[ i ]);
        SIZE += x;

        fflush(stdout);
    }

    pressedKey = 0;

    bool isQuickSave = false;
    bool isQuickLoad = false;

    while (pressedKey != '\n') {
        usleep(50000);

        if(pressedKey == 's' || pressedKey == 'S') {
            if(isQuickSave) {
                isQuickSave = false;

            }
        }

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

    print("");

    return choice;
}

void chooseAndRun(int size, choice *cs) {
    if (!HAS_INIT) {
        print("CRITICLA ERROR: Running storybox function without initialization.");
        exit(SIGINFO);
    }

    string arr[ size ];
    for (int i = 0; i < size; i++) arr[ i ] = cs[ i ].text;
    int result = choose(size, arr);
    cs[ result ].callback();
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

// needs to be ran only once at the start of the program.
// initializes keypress daemons and everything.
void __init_storybox_tools() {
    if (HAS_INIT) {
        print("ERROR: Reinitialization of storybox.");
        exit(SIGINFO); // Exit
    }

    HAS_INIT = true;
    noncannon();
    isAnyKeyPressed();
}

void clearscreen() {
    // ----------------- CLEAR SCREEN -----------------
#ifdef _WIN32
    system("cls");
#endif

#ifdef WIN32
    system("cls");
#endif

#ifdef __unix__
    system("clear");
#endif

#ifdef __APPLE__
    system("clear");
#endif
    // ----------------- CLEAR SCREEN -----------------
}
