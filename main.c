#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define CHILD_PROCESS_COUNT 5
#define MAX_WORD_COUNT 10
#define CWD_LENGTH 1024
#define DELIMITER " \n"
#define CMD_LENGTH 81
#define USER "sau"

char *CWD;
char *CMD;

/**
 * This function acquires current directory
 */
void getCurrentWordDirectory() {
    CWD = (char *) malloc(sizeof(char) * CWD_LENGTH);
    memset(CWD, 0, CWD_LENGTH);
    getcwd(CWD, CWD_LENGTH);
}

/**
 * This function initializes the CMD string
 */
void initCMD() {
    CMD = (char *) malloc(sizeof(char) * (CMD_LENGTH));
    memset(CMD, 0, CMD_LENGTH);
}

/**
 * This function prints the prompt interface
 */
void prompt() {
    printf("\033[0;32m");
    printf("%s: ", CWD);
    printf("\033[0;34m");
    printf("%s", USER);
    printf("\033[0;35m");
    printf(" > ");
    printf("\033[0m");
}

/**
 * This function obtains the command
 */
void getCommand() {
    prompt();
    fgets(CMD, CMD_LENGTH - 1, stdin);
}

/**
 * This function parses the given command
 * @param argv is the array of command tokens
 */
void parseCommand(char *argv[]) {
    int i = 0;
    char * word = strtok(CMD, DELIMITER);

    argv[i] = word;

    do {
        word = strtok(NULL, DELIMITER);
        argv[++i] = word;
    }
    while (word);

    argv[++i] = NULL;
}

/**
 * This function clears the command string to obtaining new ones
 */
void clearCommand() {
    memset(CMD, 0, CMD_LENGTH);
}

/**
 * Runs the built-in exit command
 */
void exitProcedure() {
    printf("Exit\n");
    free(CWD);
    exit(0);
}

/**
 * Runs the built-in cd command
 */
void cdProcedure() {
    char path[CMD_LENGTH] = {0};
    sscanf(CMD, "cd %s", path);
    chdir(path);
    getCurrentWordDirectory();
}

/**
 * Runs the built-in showpid command
 */
void showpidProcedure() {
    int pid;
    int status;

    for (int i = 0; i < CHILD_PROCESS_COUNT; ++i) {
        if ((pid = fork()) == 0) {
            printf("%d\n", getpid());
            exit(0);
        } else if (pid < 0) {
            printf("Process couldn't create.\n");
        } else {
            waitpid(pid, &status, WUNTRACED);
        }
    }
}

/**
 * Runs the command that is non-built-in
 */
void nonBuiltInProcedure() {
    int pid;
    int status;
    char *argv[MAX_WORD_COUNT] = {NULL};
    parseCommand(argv);

    if ((pid = fork()) == 0) {
        execvp(argv[0], argv);
        printf("Error: The command couldn't run.\n");
        exit(0);
    } else if (pid < 0) {
        printf("Process couldn't create.\n");
    } else {
        waitpid(pid, &status, WUNTRACED);
    }
}

/**
 * Runs the command by using helper functions
 */
void runCommand() {
    char firstWord[CMD_LENGTH] = {0};
    sscanf(CMD, "%s", firstWord);

    if (strcmp(firstWord, "exit") == 0) {
        exitProcedure();
    } else if (strcmp(firstWord, "cd") == 0) {
        cdProcedure();
    } else if (strcmp(firstWord, "showpid") == 0) {
        showpidProcedure();
    } else {
        nonBuiltInProcedure();
    }
}

/**
 * Runs the shell by obtaining and running commands
 */
void runShell() {
    while (1) {
        getCommand();
        runCommand();
        clearCommand();
    }
}

int main() {
    getCurrentWordDirectory();
    initCMD();
    runShell();
    return 0;
}