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
        printf("%-30s state is \t%d\n", activeDevices[i].name, activeDevices[i].state);
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
}

void disable_random (device *activeDevices, int numberOfDevices)
{
    int random;
    srand(time(NULL));

    random = rand() % (numberOfDevices);
    activeDevices[random].state = 0;
    printf("%s turned off.\n", activeDevices[random].name);
}   

int time_to_min (int t1, int t2)
{
    return t1 * 60 + t2;
}

int within_interval (int currentTime, int min2, int interval)
{
    if((currentTime / interval) * interval < min2)
        if((1 + currentTime / interval) * interval > min2)
            return 1;

    return 0;
}

int main (void)
{
    int min = 0,
        runAlready = 0,
        ready = 0,
        numberOfDevices,
        interval = 60;

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
        	/* update all states */
        	load_current_state (activeDevices, numberOfDevices);
            ftime = fopen ("time.txt","w");
            fprintf (ftime, "%d\n", min);
            fclose (ftime);
            printf("Time: %02d:%02d (%d) adding %d minutes!\n", min / 60, min % 60, min, interval);
            runAlready = 1;
            min += interval;

            /* This is a demonstration */
            if(within_interval(min, time_to_min(07,20), interval))
                activeDevices[6].state = 1;

            if(within_interval(min, time_to_min(07,30), interval))
                activeDevices[6].state = 0;

            if(within_interval(min, time_to_min(07,30), interval))
                activeDevices[6].state = 0;

            if(within_interval(min, time_to_min(07,50), interval))
                activeDevices[9].state = 1;

            if(within_interval(min, time_to_min(16,20), interval))
                activeDevices[4].state = 1;

            if(within_interval(min, time_to_min(20,50), interval))
                activeDevices[4].state = 0;

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