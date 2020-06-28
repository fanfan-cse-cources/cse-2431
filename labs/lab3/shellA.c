#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 30 /* 30 chars per line, per command, is enough. */
#define NUM_OF_HISTORY_CMD 6

char historyBuff[NUM_OF_HISTORY_CMD * MAXLINE]; /* char buffer to hold up to 6 of the most recently entered commands */
int cmdCounter = 0; /* counter for command entered */
int mostRecentCmdPos = 0; /* The most recent command position in historyBuff */
/* END */

/** The setup() routine reads in the next command line string storing it in the input buffer.
The line is separated into distinct tokens using whitespace as delimiters.  Setup also
modifies the args parameter so that it holds points to the null-terminated strings  which
are the tokens in the most recent user command line as well as a NULL pointer, indicating the end of the argument list, which comes after the string pointers that have been assigned to
args. ***/

void printCurrentCMD(printCounter, currentCMD)
{
    int currentChar = currentCMD * MAXLINE;
    printf("%d: ", printCounter);
    while (historyBuff[currentChar] != '\0')
    {
        printf("%c", historyBuff[currentChar]);
        currentChar++;
    }
}

void getHistory()
{
    int currentCMD = 0;
    int printCounter = 1;

    if (cmdCounter <= NUM_OF_HISTORY_CMD)
    {
        /* cmdCounter is less than NUM_OF_HISTORY_CMD */
        while (currentCMD < cmdCounter)
        {
            printCurrentCMD(printCounter, currentCMD);
            printCounter++;
            currentCMD++;
        }

    } else if (cmdCounter > NUM_OF_HISTORY_CMD)
    {
        /* otherwise */
        currentCMD = mostRecentCmdPos + 1;

        /* print commands after mostRecentCmdPos first to NUM_OF_HISTORY_CMD - 1 */
        while (currentCMD < NUM_OF_HISTORY_CMD)
        {
            printCurrentCMD(printCounter, currentCMD);
            printCounter++;
            currentCMD++;
        }

        /* print commands from 0 to mostRecentCmdPos */
        currentCMD = 0;
        while (currentCMD <= mostRecentCmdPos)
        {
            printCurrentCMD(printCounter, currentCMD);
            printCounter++;
            currentCMD++;
        }
    }
}

void setup(char inputBuff[], char *args[], int *background, char commandCopy[], int rerun)
{
    int length,  /* Num characters in the command line */
        i,       /* Index for inputBuff arrray          */
        j,       /* Where to place the next parameter into args[] */
        start,
        rnum = -1;   /* Beginning of next command parameter */

    /* Read what the user enters */

    if (rerun == 1) /* If rerun flag set, wipe inputBuff, then copy from commandCopy*/
    {
        printf("%s", commandCopy);

        memset(inputBuff, '\0', sizeof(MAXLINE));
        strcpy(inputBuff, commandCopy);

        length = strlen(commandCopy);
    } 
    else if (strncmp(inputBuff, "r", 1) == 0) /* if entered rnum, , wipe inputBuff, then get that statement and copy to inputBuff */
    {
        int commandNum = inputBuff[1] - 48;
        memset(inputBuff, '\0', sizeof(MAXLINE));

        /* cmdCounter is less than NUM_OF_HISTORY_CMD */
        if (cmdCounter <= NUM_OF_HISTORY_CMD)
        {
            int headOfCMD = (commandNum - 1) * MAXLINE;
            int CMDCharOffset = 0;

            while (historyBuff[headOfCMD + CMDCharOffset] != '\0')
            {
                inputBuff[CMDCharOffset] = historyBuff[headOfCMD + CMDCharOffset];
                CMDCharOffset++;
            }

            length = strlen(inputBuff);
        }
        else
        {
            int headOfCMD = 0;
            int CMDCharOffset = 0;

            if (mostRecentCmdPos + commandNum < NUM_OF_HISTORY_CMD)
            {
                headOfCMD = (mostRecentCmdPos + commandNum) * MAXLINE;
            }
            else if (commandNum == 6)
            {
                headOfCMD = mostRecentCmdPos * MAXLINE;
            }
            else {
                headOfCMD = (NUM_OF_HISTORY_CMD - commandNum - 1) * MAXLINE;
            }

            while (historyBuff[headOfCMD + CMDCharOffset] != '\0')
            {
                inputBuff[CMDCharOffset] = historyBuff[headOfCMD + CMDCharOffset];
                CMDCharOffset++;
            }

            length = strlen(inputBuff);
        }
        printf("%s", inputBuff);
        rnum = 1;
    } else
    {
        length = read(STDIN_FILENO, inputBuff, MAXLINE);
    }

    j = 0;
    start = -1;
    if (length == 0)
        exit(0); /* Cntrl-d was entered, end of user command stream */
    if (length < 0) {
        perror("error reading command");
        exit(-1); /* Terminate with error code of -1 */
    }

    if (rerun == 0 && rnum != 1 && (strncmp(inputBuff, "rr", 2) != 0) && (inputBuff[0] != '\n'))
    {
        if ((strncmp(inputBuff, "history", 7) != 0) && (strncmp(inputBuff, "h", 1) != 0))
        {
            mostRecentCmdPos = cmdCounter % NUM_OF_HISTORY_CMD;
            int headOfCMD = mostRecentCmdPos * MAXLINE;
            int CMDCharOffset = 0;

            memset(commandCopy, '\0', sizeof(MAXLINE));

            while (inputBuff[CMDCharOffset] != '\0')
            {
                historyBuff[headOfCMD + CMDCharOffset] = inputBuff[CMDCharOffset];
                commandCopy[CMDCharOffset] = inputBuff[CMDCharOffset];
                CMDCharOffset++;
            }

            cmdCounter++;
        } else {
            getHistory();

            printf("rr/rnum: ");
            memset(inputBuff, '\0', sizeof(MAXLINE));
            scanf("%s", inputBuff);
        }
    }

    /* Examine every character in the input buffer */
    for (i = 0; i < length; i++) {
        switch (inputBuff[i]) {
        case ' ':
        case '\t' : /* Argument separators */

            if (start != -1) {
                args[j] = &inputBuff[start]; /* Set up pointer */
                j++;
            }

            inputBuff[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;

        case '\n': /* Final char examined */
            if (start != -1) {
                args[j] = &inputBuff[start];
                j++;
            }

            inputBuff[i] = '\0';
            args[j] = NULL; /* No more arguments to this command */
            break;

        case '&':
            *background = 1;
            inputBuff[i] = '\0';
            break;

        default : /* Some other character */
            if (start == -1)
                start = i;
        }
    }

    args[j] = NULL; /* Just in case the input line was > 40 */
}

int main(void)
{
    char inputBuff[MAXLINE]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE / 2 + 1]; /* Command line arguments */
    int background; /* Equals 1 if a command is followed by '&', else 0 */
    char commandCopy[MAXLINE]; /* Array to store copy of command in setup function */
    int rerun = 0;

    while (1) { /* Program terminates normally inside setup */
        background = 0;
        pid_t pid;

        printf("CSE2431Sh$ "); /* Shell prompt */
        fflush(0);

        setup(inputBuff, args, &background, commandCopy, rerun); /* Get next command */

        if (strncmp(inputBuff, "rr", 2) == 0)
        {
            rerun = 1;
        } else {
            rerun = 0;
        }

        pid = fork();

        if (pid < 0) {
            printf ("ERROR: fork failed!\n");
            exit (0);
        }
        else if (pid == 0) execvp(args[0], args);
        else {
            if (background == 0) waitpid(pid);
            else sleep(1);
        }
    }

    return (0);
}
