/*
 * B2-28a
 * Version 1.0 - Initial Concept  
 * 24-11-2013
 *
 * This is the simulator part of the program.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#define MAX_DEVICES         100

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define OS                  1
#include <windows.h> 
#else
#define OS                  0
#include <unistd.h>
int Sleep(int sleepMs) {return sleep(sleepMs / 1000);}
#endif

typedef struct
{
    int min,
        id,
        setTo;
} state;

typedef struct {
    char name[50];
    int id,
        state;
} device;

/* Same load_devices, load_current_state, write_current_state as in main.c */
int load_devices (FILE *devices, device *activeDevices)
{
    char ch;    /* Char to check if the next char is EOF    */
    int n = 0;  /* Count number of devices loaded           */

    /* While the next char isn't EOF read devices from file     */
    while (((ch = fgetc(devices)) != EOF) && ungetc(ch, devices)) 
    {
        fscanf(devices, "%s ", activeDevices[n].name);
        
        fscanf(devices, "%d ", &activeDevices[n].id);
        
        activeDevices[n].state = 0;

        n++;
    }

    return n;
}

void load_current_state (device *activeDevices, int numberOfDevices)
{
    int i;
    char filename[50];
    FILE * tmp;
    for (i = 0; i < numberOfDevices; i++)
    {
        sprintf(filename, "%d", activeDevices[i].id);

        tmp = fopen(filename, "r");
        /* Exit on failure */
        if (tmp == NULL)
        {
            printf("Loading file %d failed. Quitting.\n", activeDevices[i].id);
            exit(1);
        }
        fscanf (tmp, "%d", &activeDevices[i].state);
        fclose (tmp);
        printf("%s state is %d\n", activeDevices[i].name, activeDevices[i].state);
    }
}

void write_current_state (device *activeDevices, int numberOfDevices)
{
    int i;
    char filename[50];
    FILE * tmp;
    for (i = 0; i < numberOfDevices; i++)
    {
        sprintf(filename, "%d", activeDevices[i].id);
        tmp = fopen(filename, "w");
        fprintf (tmp, "%d\n", activeDevices[i].state);
        fclose (tmp);
    }
}

void enable_random (device *activeDevices, int numberOfDevices)
{
    int random;
    srand(time(NULL));

    random = rand() % (numberOfDevices);
    activeDevices[random].state = 1;
    printf("%s turned on.\n", activeDevices[random].name);

    random = rand() % (numberOfDevices);
    activeDevices[random].state = 0;
    printf("%s turned off.\n", activeDevices[random].name);
}

int main (void)
{
    int min = 0,
        runAlready = 0,
        ready = 0,
        numberOfDevices;

    FILE * ftime;
    FILE * start;
    FILE * devicesIn;

    device *activeDevices;

    printf("Waiting for main to start simulation.\n");

    /* Wait for main to be ready */
    while (!ready)
    {
        if((int) time(NULL) % 2 == 1)
        {
            start = fopen ("start", "r");
            if (start != NULL)
               ready = 1;
        }

        Sleep(1000);
    }

    activeDevices = (device *) calloc (MAX_DEVICES, sizeof(device));
    devicesIn = fopen ("devices_test.txt", "r");
    numberOfDevices = load_devices (devicesIn, activeDevices);
    fclose (devicesIn);
    load_current_state (activeDevices, numberOfDevices);
    
    while (min < 1450)
    {
        if(!runAlready && (int) time(NULL) % 2 == 1)
        {
            ftime = fopen ("time.txt","w");
            fprintf (ftime, "%d\n", min);
            fclose (ftime);
            printf("Time: %02d:%02d (%d) adding 60 minutes!\n", min / 60, min % 60, min);
            runAlready = 1;
            min += 60;

            enable_random (activeDevices, numberOfDevices);

            write_current_state (activeDevices, numberOfDevices);

        }

        if((int) time(NULL) % 2 == 0)
        {
            runAlready = 0;
        }

        Sleep(1000);
    }

    remove("time.txt");

    return 0;
}