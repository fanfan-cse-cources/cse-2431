#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* YIFAN YAO */

#define MAXLINE 50 /* 50 chars per line, per command, should be enough. */

/**
The setup() routine reads in the next command line string storing it in the input buffer.
The line is separated into distinct tokens using whitespace as delimiters.  Setup also 
modifies the args parameter so that it holds points to the null-terminated strings  which 
are the tokens in the most recent user command line as well as a NULL pointer, indicating the
end of the argument list, which comes after the string pointers that have been assigned to
args. 
**/

/** 
PUT A COMMENT BELOW WHICH DESCRIBES HOW THE setup FUNCTION PARSES THE inputBuff ARRAY INTO arguments.

HOW setup PARSES inputBuff INTO ARGUMENTS:
 (1) Read one line from the standard input, the length of the input is MAXLINE,
 (2) If the length is a illegal value, exit with error,
 (3) Arguements are separted by ' ' and '\t',
 (4) '\n' should be the end of the input,
 (5) If '&' present, background will set to 1,
 (6) Other characters will put into args[] and waiting for execution;
**/

void setup(char inputBuff[], char *args[],int *background)
{
    int length,  /* Num characters in the command line */
        i,       /* Index for inputBuff arrray          */
        j,       /* Where to place the next parameter into args[] */
        start;   /* Beginning of next command parameter */

    /* Read what the user enters */
    length = read(STDIN_FILENO, inputBuff, MAXLINE);  

    j = 0;
    start = -1;

    if (length == 0)
        exit(0);            /* Cntrl-d was entered, end of user command stream */

    if (length < 0){
        perror("error reading command");
    exit(-1);           /* Terminate with error code of -1 */
    }
    
    /* Examine every character in the input buffer */
    for (i = 0; i < length; i++) {
 
        switch (inputBuff[i]){
        case ' ':
        case '\t' :          /* Argument separators */

            if(start != -1){
                args[j] = &inputBuff[start];    /* Set up pointer */
                j++;
            }

            inputBuff[i] = '\0'; /* Add a null char; make a C string */
            start = -1;
            break;

        case '\n':             /* Final char examined */
            if (start != -1){
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
            
        default :             /* Some other character */
            if (start == -1)
                start = i;
    }
 
    }    
    args[j] = NULL; /* Just in case the input line was > 50 */
} 

int main(void)
{
    char inputBuff[MAXLINE]; /* Input buffer  to hold the command entered */
    char *args[MAXLINE/2+1];/* Command line arguments */
    int background;         /* Equals 1 if a command is followed by '&', else 0 */
    pid_t ret_val;
    pid_t pid;
    

    while (1)
    {            /* Program terminates normally inside setup if appropriate */

    background = 0;

    printf("C2431Sh$ ");  /* Shell prompt */
        fflush(0);

        setup(inputBuff, args, &background);       /* Get next command */

    /* 
    Fill in the code for these steps:  
     (1) Fork a child process using fork(),
     (2) The child process will execute the command by invoking execvp(),
     (3) If bkgnd == 0, the parent will wait; 
        otherwise returns to top of loop to call the setup() function.
    */

    ret_val = fork();

    /* If ret_val is negative, child process did not create */
    if (ret_val < 0)
    {
        printf("Error on fork.\n");
        exit (1);
    }

    /* Child process execute commands */
    if (ret_val == 0)
    {
        execvp(args[0], args);
    }
    /* Parent process */
    else if (ret_val > 0) 
    {
        /* If '&' present (background == 1), parent and child process will run
         concurrently */
        if (background == 0) 
        {
            wait(NULL);
        }
        else
        {
        	sleep(1);
            continue;
        }
    }
    }
}
