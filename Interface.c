//Alex Foggia
//Project 3

//Interface program that communicates with user

#include "MPGHeader.h"

int main()
{
    char args1[50], args2[50];  //Character arrays for the arguments to be passed to the server
    char interface_RBuff[BUFFER_SIZE+1], interface_WBuff[BUFFER_SIZE+1];  //Buffers used for read and write (Interface only)
    int finishStatus = INITIAL_CONST;  //Variable used just for status on waitpid()
    int pipeToServ[2];  //Array that holds the read and write pipe to the server
    int pipeToInter[2];  //Array that holds the read and write pipe to the interface
    int serverPid;  //Variable for the server process ID
    int execError;  //Error variable for execl command and waitpid
    int error_With_Write, error_With_Read;  //Variables used to detect if there are errors in the strings read or wrote to server or interface
    int loopFinish = INITIAL_CONST;  //Variable that controls the server operation loop

    pipe(pipeToServ);  //Creates the pipe to the server
    pipe(pipeToInter);  //Creates the pipe to the interface

    serverPid = fork();  //Fork child process

    checkErrors(serverPid);

    if (serverPid == 0)  //If in child process, initialize the program and server
    {

        close(pipeToServ[1]);  //Close pipe end to server prior to use
        close(pipeToInter[0]);  //Close pipe end to interface prior to use

        printf("Server is initializing...\n");

        sprintf(args1,"%d", pipeToServ[0]);  //Give arguments to server pipe
        sprintf(args2,"%d", pipeToInter[1]);  //Give arguments to interface pipe

        execError = execl("./Server", args1, args2, NULL);  //Start up server program

        checkErrors(execError);  //Error check to make sure server started up correctly

    }

    else  //Otherwise, in parent process where commands are excepted and executed
    {

        close(pipeToServ[0]);  //Close pipe end to server prior to use
        close(pipeToInter[1]);  //Close pipe end to interface prior to use

        printf("Reading from Server\n\n");
        error_With_Read = read(pipeToInter[0], interface_RBuff, 99);  //Read in the initialization from the server
        printf("Response: %s", interface_RBuff);
        clearBuffer(interface_RBuff);
        checkErrors(error_With_Read);

        while(loopFinish == 0)
        {

            printf("=> ");

            clearBuffer(interface_WBuff);
            scanf("%s", interface_WBuff);  //Scan for the user command to put in write buffer
            error_With_Write = write(pipeToServ[1], interface_WBuff, BUFFER_SIZE);  //Write the command to the server
            checkErrors(error_With_Write);

            error_With_Read = read(pipeToInter[0], interface_RBuff, BUFFER_SIZE);  //Read the servers response
            checkErrors(error_With_Read);

            if(atoi(interface_RBuff) == 1)  //Check to see if the server has exited
            {
                loopFinish = 1;
                sprintf(interface_RBuff, "%s", "Server complete.\n");
                printf("Response: %s", interface_RBuff);
                printf("Interface: Child process (%d) completed.\n", serverPid);
                break;
            }

            printf("Response: %s\n", interface_RBuff);

            clearBuffer(interface_RBuff);

        }

        execError = waitpid(-1, &finishStatus, 0);  //Wait for the child process
        checkErrors(execError);

        printf("Interface: Child process exit status = %d.\n", finishStatus);  //Display exit status
        printf("Interface: Complete.\n");

    }

    return 0;
}
