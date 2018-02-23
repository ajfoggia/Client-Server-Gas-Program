//
// Created by ajfoggia on 2/12/18.
//

#ifndef MPGHEADER_H
#define MPGHEADER_H

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
 //Limit for the number of cars in the file
#define BUFFER_SIZE 2048  //Max buffer size
#define INITIAL_CONST 0  //Constant used for initialization

//Structure declaration for the car information.
typedef struct
{
    char id[8]; //id of vehicle
    int odometer; //number of miles on odometer
    float gallons; //gallons
}carInfo;

void checkErrors(int error)  //Used to check if an error occurred with reading or writing from buffers.
{

    if(error == -1)
    {
        printf("It looks like there was an issue...");
        printf("Error Number :%d\n", errno);
        exit(-1);
    }
}

void clearBuffer(char *buf_Input)  //Clears the buffer after use.
{
    for(int buf_Index = 0; buf_Index < BUFFER_SIZE; buf_Index++)
    {
        *(buf_Input+buf_Index) = '\0';
    }
}

void carListSort(carInfo *car, int totalCars)  //Sorts the array of car structures.
{

    int outerIndex, innerIndex;

    for (outerIndex = 0; outerIndex < (totalCars -1); outerIndex++ )
    {
        for(innerIndex = 0; innerIndex < (totalCars - outerIndex - 1); innerIndex++)
        {
            if(car[innerIndex].odometer > car[innerIndex+1].odometer)  //If the current car has a greater odometer then the next car, swap them
            {
                carInfo temp;
                temp = car[innerIndex];
                car[innerIndex] = car[innerIndex+1];
                car[innerIndex+1] = temp;
            }
        }
    }
}


float totalMPG(char *info, const carInfo *car, int carTotal) //Calculates the miles per gallon for a given vehicle.
{
    float totalOdom = 0.0;  //Variable that keeps track of total miles
    float totalGal = 0.0;  //Variable that keeps track of total gallons used
    float finalOdom = 0.0;  //Variable that holds that average MPG
    for(int i = 0; i < carTotal; i++)  //For however many cars were read in
    {

        if(strcmp(car[i].id, info) == 0)
        {
            if(car[i].gallons == 0.0)  //If no gallons were added then the total odometer stays the same
                finalOdom = car[i].odometer;
            else  //Otherwise, gallons were added and so the amount of miles are calculated
            {
                totalOdom += (car[i].odometer - finalOdom); //Total miles put on the car
                totalGal += car[i].gallons;  //Total gallons put in the car
                finalOdom = car[i].odometer;  //Final odometer reading
            }
        }
    }

    if(totalGal != 0)
        return totalOdom/totalGal;  //Return the MPG
    else
        return 0.0;
}

int countWords(FILE *infile)
{
    int count = 0;
    char character;
    while ((character = fgetc(infile)) != EOF)
    {
        if (character == '\n')
            count++;
    }
    return count;
}

#endif //MPGHEADER_H
