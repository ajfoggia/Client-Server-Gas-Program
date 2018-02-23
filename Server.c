//Alex Foggia
//Project 3

//Server program that handles all the operations "behind the scenes".

#include "MPGHeader.h"

int main(int argc, char **argv)
{
    int MAX_CARS = 0;
    char server_WBuf[BUFFER_SIZE];  //Buffer to transfer the written strings through the pipes
    char server_RBuf[BUFFER_SIZE];  //Buffer to transfer the read strings through the pipes
    int pipeToServ[2], pipeToInter[2];  //Array that holds the read and write pipe to the server and interface
    int fileError;  //Error variable while reading the file
    int Done = INITIAL_CONST;  //Variable that terminates the while loop
    int numOfCars = INITIAL_CONST;  //Total number of cars being read in
    int error_With_Write, error_With_Read;  //Variables used to detect if there are errors in the strings read or wrote to server or interface
    int *time = 10;  //Int pointer constant to fix a warning flag at compile time

    FILE *inFile;  //Input file variable

    sscanf(argv[0], "%d", &pipeToServ[0]);  //Creates pipe that communicates to server.
    sscanf(argv[1], "%d", &pipeToInter[1]);  //Creates pipe that communicates to interface.

    inFile = fopen("SmallGas","r");  //Opens file "gasData" for counting the number of lines to read

    MAX_CARS = countWords(inFile);  //Save the number into MAX_CARS

    carInfo carList[MAX_CARS + 1];  //Create the array of car structures

    inFile = fopen("SmallGas", "r");  //Reopen the file for usage

    for(int i = INITIAL_CONST; !feof(inFile) && i < MAX_CARS; i++)  //Loop to read the carInfo from the file
    {
        fileError = fscanf(inFile, "%7s %i %f \n", carList[i].id, &carList[i].odometer, &carList[i].gallons);
        checkErrors(fileError);
        printf("Element # = [%2d]: Car ID = %7s, Odometer # = %6i, # of Gallons = %9f \n", i, carList[i].id, carList[i].odometer, carList[i].gallons);  //Prints each car instance
        numOfCars++;
    }

    carListSort(carList, numOfCars);  //Sorts the car info for when list command is called
    clearBuffer(server_WBuf);  //Clear write buffer for future input

    sprintf(server_WBuf, "\n   The file was added successfully.\n   Server can now accept commands:\n");  //String loaded into write buffer
    error_With_Write = write(pipeToInter[1], server_WBuf, 99);  //Write above string to interface
    checkErrors(error_With_Write);  //Error check to make sure string was successfully printed

    while(Done == 0)  //While loop to allow multiple commands to be sent and evaluated from interface to server and vice versa
    {

        error_With_Read = read(pipeToServ[0], server_RBuf, BUFFER_SIZE);  //Read the command typed in by the user in the interface.
        clearBuffer(server_WBuf);
        checkErrors(error_With_Read);  //Check if input that was read does not yield error

        if(strncmp(server_RBuf, "\0", 1) == 0)  //If nothing is entered then wait for input
        {
            wait(time);
        }

        else if(strncmp(server_RBuf, "mpg,", 4)== 0 )  //If mpg command is entered, then evaluate the miles per gallon for a given ID
        {

            char carID[8];  //Variable to hold the car ID
            int position = INITIAL_CONST;  //Variable that keeps the position of the array carID
            float updateMPG = 0;  //Variable to hold the updated miles per gallon

            for (int i = 4; i < 16; i++ )
            {
                carID[position++] = server_RBuf[i];  //Put the ID string into the array from the readBuffer
            }

            clearBuffer(server_RBuf);

            updateMPG = totalMPG(carID, carList, numOfCars);  //Calculate the miles per gallons for the vehicle

            //These help print the result in the corresponding fashion to the interface
            clearBuffer(server_WBuf);
            sprintf(server_WBuf,"\nAverage MPG for %s = %f.", carID, updateMPG);
            error_With_Write = write(pipeToInter[1], server_WBuf, 99);
            checkErrors(error_With_Write);

        }

        else if (strncmp(server_RBuf, "list,", 5) == 0 )  //If list command is entered, then display the cars in sorted fashion (which was done after cars were read in)
        {

            char given_ID[8];
            int finish = INITIAL_CONST;
            int i = INITIAL_CONST;
            int j = INITIAL_CONST;
            int offset = INITIAL_CONST;

            for(i = 5; i < 16; i++)
            {
                given_ID[j++] = server_RBuf[i];  //Reads in the id portion of the input
            }


            clearBuffer(server_RBuf);


            if(numOfCars == 0)  //If there are no cars left, then the condition is finished
            {
                finish = 1;
            }

            clearBuffer(server_WBuf);

            for(i = INITIAL_CONST; (i < numOfCars) && (finish != 1); i++)  //Loop to collect all the cars and their info
            {
                if(strcmp(carList[i].id, given_ID) == 0)  //If the ID is correct, add it to the sorted list
                {
                    offset += sprintf(server_WBuf+offset,"\n[%2d]:%7s %6i %9f", i, carList[i].id , carList[i].odometer, carList[i].gallons);
                }
            }

            error_With_Write = write(pipeToInter[1], server_WBuf, BUFFER_SIZE);  //Writes the finished sorted list to interface
            checkErrors(error_With_Write);
        }

        else if(strncmp(server_RBuf, "exit", 4) == 0)  //If exit command is entered, send 1 down the pipe.
        {

            clearBuffer(server_WBuf);
            sprintf(server_WBuf, "%d", 1);  //Fill the buffer with 1
            error_With_Write = write(pipeToInter[1], server_WBuf, 99); //Send 1 to the interface for termination of program.
            checkErrors(error_With_Write);  //Make sure termination was successfully written
            Done = 1;  //Can break from loop

        }

        else  //Command not accepted, displays error message and attempts to get another command
        {

            clearBuffer(server_WBuf);
            sprintf(server_WBuf,"\nI'm sorry, but the command you entered was not recognized. Please try again.");
            error_With_Write = write(pipeToInter[1], server_WBuf, 99);
            checkErrors(error_With_Write);

        }

        clearBuffer(server_RBuf);  //Clears readBuffer for another command attempt

    }

    return 0;

}
