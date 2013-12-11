/*
 * B2-28a
 * Version 0.2 - Initial idea (Indev) 
 * 24-11-2013
 *
 * Outline for file formats located in the end of this file.
 * Variables i and j are used as counters in loops etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> 

#define MAX_RULES           100
#define MAX_DEVICES         100
#define DEVICES_PR_RULE     10


/* In order to secure compatibility with both windows and Linux the code bellow is used.*/ 
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define OS                  1
#include <windows.h> 
#else
#define OS                  0
#include <unistd.h>
int Sleep(int sleepMs) {return sleep(sleepMs / 1000);}
#endif

typedef struct {
    char name[50];
    int dependencies[DEVICES_PR_RULE],      /* devices (id) on which the rule depend upon   */
        reactantsEnable[DEVICES_PR_RULE],   /* devices (id) on which the rule enables       */
        reactantsDisable[DEVICES_PR_RULE],  /* devices (id) on which the rule disables      */
        active,                             /* 0 = inactive                                 */
        timerBased,                         /* if 1, can also be actived by timer           */
        min;                                /* Time to start (in minutes 0-1440 if enabled  */
} rule;

typedef struct {
    char name[50];
    int id,
        state;
} device;

int load_rules (FILE *rules, rule *activeRules)
{
    char ch;    /* Char to check if the next char is EOF    */
    int n = 0,  /* Count number of rules loaded             */
        i;

    /* While the next char isn't EOF read rules from file   */
    while (((ch = fgetc(rules)) != EOF) && ungetc(ch, rules)) 
    {
        fscanf(rules, " %s", activeRules[n].name);

        fscanf(rules, " %d", &activeRules[n].active);

        fscanf(rules, " %d", &activeRules[n].timerBased);

        fscanf(rules, " %d", &activeRules[n].min);

        for (i = 0; i < DEVICES_PR_RULE; i++) 
        {
            fscanf (rules, " %d ", &activeRules[n].dependencies[i]);
        }

        for (i = 0; i < DEVICES_PR_RULE; i++) 
        {
            fscanf (rules, " %d ", &activeRules[n].reactantsEnable[i]);
        }

        for (i = 0; i < DEVICES_PR_RULE; i++) 
        {
            fscanf (rules, " %d ", &activeRules[n].reactantsDisable[i]);
        }

        n++;
    }

    return n;
}

void write_rules (FILE *rules, rule *activeRules, int numberOfRules) 
{
    int i, j;

    for (i = 0; i < numberOfRules; i++)
    {
        fprintf(rules, "%s\n", activeRules[i].name);

        fprintf(rules, "%d\n", activeRules[i].active);

        fprintf(rules, "%d\n", activeRules[i].timerBased);

        fprintf(rules, "%d\n", activeRules[i].min);
        
        for (j = 0; j < DEVICES_PR_RULE; j++)
        {
            fprintf(rules, "%d ", activeRules[i].dependencies[j]);
        }
        
        fprintf(rules, "\n");

        for (j = 0; j < DEVICES_PR_RULE; j++)
        {
            fprintf(rules, "%d ", activeRules[i].reactantsEnable[j]);
        }

        for (j = 0; j < DEVICES_PR_RULE; j++)
        {
            fprintf(rules, "%d ", activeRules[i].reactantsDisable[j]);
        }
        
        fprintf(rules, "\n\n");
    }
}

void print_single_rule (rule *activeRules, int ruleNumber) 
{
    int i;
    printf("Rule number %d\n", ruleNumber);
    printf("Name:\t\t%s\n", activeRules[ruleNumber].name);

    printf("Active:\t\t%d\n", activeRules[ruleNumber].active);

    printf("Timer based:\t%d\n", activeRules[ruleNumber].timerBased);
    if (activeRules[ruleNumber].timerBased == 1) 
    {
        printf("Active at time:\t%02d:%02d\n", 
                activeRules[ruleNumber].min / 60, activeRules[ruleNumber].min % 60);
    }

    printf("Depends on devices:\n");        
    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        printf("%d ", activeRules[ruleNumber].dependencies[i]);
    }
    
    printf("\nAffects devices (Enables):\n");

    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        printf("%d ", activeRules[ruleNumber].reactantsEnable[i]);
    }

    printf("\nAffects devices (Disables):\n");

    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        printf("%d ", activeRules[ruleNumber].reactantsDisable[i]);
    }
    
    printf("\n\n");
}

int add_rule (rule *activeRules, int numberOfRules) 
{
    int i;

    printf("Enter a name for the rule (no spaces):\n>");
    scanf("%s", activeRules[numberOfRules].name);
    activeRules[numberOfRules].active = 1;
    activeRules[numberOfRules].timerBased = 0;
    activeRules[numberOfRules].min = 0;

    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        activeRules[numberOfRules].dependencies[i] = -1;
        activeRules[numberOfRules].reactantsEnable[i] = -1;
        activeRules[numberOfRules].reactantsDisable[i] = -1;
    }

    return numberOfRules + 1;
}

int delete_rule (rule *activeRules, int numberOfRules)
{
    int ruleNumber, i, j;;
    do {
        printf("Enter the number of the rule which is to be deleted: (-1 to quit)\n");
        for (i = 0; i < numberOfRules; i++)
        {
            printf("%d: %s\n", i, activeRules[i].name);
        }
        printf(">");
        scanf("%d", &ruleNumber);
        if (ruleNumber > numberOfRules - 1 || ruleNumber < - 1)
        {
            printf("Rule not found\n");
        } else if ( ruleNumber == - 1) 
        {
            return numberOfRules;
        }

    } while (ruleNumber > numberOfRules - 1 || ruleNumber < - 1);

    /* 
     * Deletion of a single array element is the biggest disadvantage of arrays ... 
     * Replaces the current element of each in the struct of the next.
     * Looped to the end of the array.
     */

    for ( i = ruleNumber; i < numberOfRules - 1; i++ )
    {
        strcpy (activeRules[i].name, activeRules[i + 1].name);
        activeRules[i].active = activeRules[i + 1].active;
        activeRules[i].timerBased = activeRules[i + 1].timerBased;
        activeRules[i].min = activeRules[i + 1].min;

        for (j = 0; j < DEVICES_PR_RULE; j++)
        {
            activeRules[i].dependencies[j] = activeRules[i + 1].dependencies[j];
            activeRules[i].reactantsEnable[j] = activeRules[i + 1].reactantsEnable[j];
            activeRules[i].reactantsDisable[j] = activeRules[i + 1].reactantsDisable[j];
        }
    }

    return numberOfRules - 1;
}

void edit_rule (rule *activeRules, int numberOfRules)
{
    int ruleNumber;
    do {
        printf("Enter the number of the rule which is to be edited: (-1 to quit)\n>");
        scanf("%d", &ruleNumber);
        if (ruleNumber > numberOfRules || ruleNumber < - 1)
        {
            printf("Rule not found\n");
        } 
            else if ( ruleNumber == - 1) 
        {
            /* void function return with no value to exit */
            return;
        }

    } while (ruleNumber > numberOfRules || ruleNumber < - 1);

   /* 
    * TO-BE-IMPLEMENTED (AFTER ADD_RULE IS DONE)
    */ 


}

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

void write_devices (FILE *devices, device *activeDevices, int numberOfDevices) 
{
    int i;

    for (i = 0; i < numberOfDevices; i++)
    {
        fprintf(devices, "%s\n", activeDevices[i].name);
        
        fprintf(devices, "%d\n", activeDevices[i].id);

    }
}

void print_single_device (device *activeDevices, int deviceNumber) 
{
    printf("Name:\t%s\n", activeDevices[deviceNumber].name);

    printf("Id:\t%d\n", activeDevices[deviceNumber].id);

    printf("State:\t%d\n\n", activeDevices[deviceNumber].state);
}

int add_device (device *activeDevices, int numberOfDevices) 
{
    /* Must be unique */
    int i;
    char tmp[50];
    printf ("Enter a name for the device (no spaces):\n>");
    scanf ("%s", tmp);
    for (i = 0; i < numberOfDevices; i++)
    {
        if (strcmp (tmp, activeDevices[i].name) == 0)
        {
            printf("Device's name already exists. Failed to create device.\n");
            return numberOfDevices;
        }
    }

    strcpy (activeDevices[i].name, tmp);
    
    activeDevices[numberOfDevices].state = 0;

    if (numberOfDevices > 0)
        activeDevices[numberOfDevices].id = activeDevices[numberOfDevices - 1].id + 1;
    else
        activeDevices[numberOfDevices].id = 0;

    return numberOfDevices + 1; 
}

int delete_device (device *activeDevices, int numberOfDevices)
{
    /* To delete by id, the id corresponding to the locations in the array must be found */
    int idInArray[MAX_DEVICES], i, deviceNumber, option = -2;
    for (i = 0; i < numberOfDevices; i++)
    {
        idInArray[i] = activeDevices[i].id;
        printf("%d\n", i);
    }

    do {
        printf("Enter the id of the device which is to be deeted: (-1 to quit)\n");
        for (i = 0; i < numberOfDevices; i++)
        {
            printf("%d: %s\n", activeDevices[i].id, activeDevices[i].name);
        }
        printf(">");
        scanf("%d", &deviceNumber);

        if (deviceNumber == -1)
        {
            return numberOfDevices;
        }

        for (i = 0; i < numberOfDevices; i++)
        {
            if (idInArray[i] == deviceNumber)
            {
                option = i;
            }
        }

        /* if the option variable haven't been set by the above loop, it's -2 */
        if (option == -2)
        {
            printf("Rule not found, try again\n");
        } 

    } while (option < 0);

    for ( i = option; i < numberOfDevices - 1; i++ )
    {
        strcpy (activeDevices[i].name, activeDevices[i].name);
        activeDevices[i].state = activeDevices[i + 1].state;
        activeDevices[i].id = activeDevices[i + 1].id;
    }

    return numberOfDevices - 1;
}

/* Rename io */
void save_files (rule *activeRules, int numberOfRules, device *activeDevices, int numberOfDevices)
{
    /* Open files in write mode and write all rules and devices*/
    FILE * rulesOut, * devicesOut;

    rulesOut = fopen ("rules_test.txt", "w");
    write_rules (rulesOut, activeRules, numberOfRules);
    printf ("Rules written (%d)\n", numberOfRules);
    fclose (rulesOut);

    devicesOut = fopen ("devices_test.txt", "w");
    write_devices (devicesOut, activeDevices, numberOfDevices);
    printf ("Devices written (%d)\n", numberOfDevices);
    fclose (devicesOut); 
}

int get_current_time_in_minutes (void)
{
    /* This function would in the real program get the current time from the OS, 
     * but in the simulation this is done via a file written by the simulatior
     */
    
    int min;
    FILE * fcurrentTime = fopen ("time.txt", "r");

    /* Check if the file exists */
    if(fcurrentTime == NULL) 
    {
        printf ("Time file doesn't exist. Open the simulator and try again\n");
        exit(1);      
    }

    fscanf(fcurrentTime, "%d", &min);
    fclose(fcurrentTime);
    return min;
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

void trigger_rule (rule *activeRules, int ruleNumber, device *activeDevices, int numberOfDevices)
{
    int i, j;
    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        for (j = 0; j < numberOfDevices; j++)
        {
            if (activeDevices[j].id == activeRules[ruleNumber].reactantsEnable[i])
            {
                activeDevices[j].state = 1;
                printf("Rule %s actived device %s\n", activeRules[ruleNumber].name, activeDevices[j].name);
            }

            if (activeDevices[j].id == activeRules[ruleNumber].reactantsDisable[i])
            {
                activeDevices[j].state = 0;
                printf("Rule %s deactivated device %s\n", activeRules[ruleNumber].name, activeDevices[j].name);
            }
        }
    }
}

int check_rule_by_state (rule *aR, int rN, device *aD, int NoD)
{
    int i,
        j,
        depAchived = 0;     /* All depencenes must be accounted for */

    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        if (aR[rN].dependencies[i] == -1)
        {
            depAchived++;
        } else {
            for (j = 0; j < NoD; j++)
            {
                if (aR[rN].dependencies[i] == aD[j].id)
                {
                    depAchived++;
                }
            }
        }
    }

    return (depAchived == 10);
}

void automation_loop (rule *activeRules, int numberOfRules, device *activeDevices, int numberOfDevices)
{
    /* Checks if the time (in seconds) is equal or odd, if equal it performs checks actions
     * This is done to avoid having both programs (the simulator and this) open a given file at the same time.
     * The current time. 
     */

    /* Get the time in order to compare it later */
    int i,
        min1 = get_current_time_in_minutes(),
        min2,
        runAlready = 0,
        cnt = 0;

    while(min1 != 0 && min1 < 510)
    {        
        /* If a check haven't occurred and the time is equal check */
        if(!runAlready && (int) time(NULL) % 2 == 0)
        {
            printf("Checking ... %d\n", (int) time(NULL));

            /* Read current io from files */
            load_current_state (activeDevices, numberOfDevices);

            /* Checks if any timerBased rules should trigger */
            min2 = get_current_time_in_minutes();
            for (i = 0; i < numberOfRules; i++)
            {
                if(activeRules[i].timerBased && (activeRules[i].min >= min1 && activeRules[i].min < min2))
                {
                    printf("%s triggered by time.\n", activeRules[i].name);
                    trigger_rule(activeRules, i, activeDevices, numberOfDevices);
                }
            }

            /* reset min1 */
            min1 = min2;

            /* check if any state based rules should trigger */
            for (i = 0; i < numberOfRules; i++)
            {
                if (check_rule_by_state (activeRules, i, activeDevices, numberOfDevices))
                {
                    printf("%s triggered by state.\n", activeRules[i].name);
                    trigger_rule(activeRules, i, activeDevices, numberOfDevices);
                }
            }

            /* Write to files ones rules have been triggered */
            write_current_state (activeDevices, numberOfDevices);

            runAlready = 1;
            cnt++;
        }

        /* If time is odd, set the check variable to 0 */
        if ((int) time(NULL) % 2 == 1) {
            runAlready = 0;
        }

        /* This sleeps the thread avoiding looping though this 
         * while loop many time unnecessarily (causes cpu load) */
        Sleep(1000);
    }
}

void automation_init (rule *activeRules, int numberOfRules, device *activeDevices, int numberOfDevices)
{
    FILE * start;
    time_t time_start;
    
    printf("Saving all rules [%d] and devices [%d]\n", numberOfRules, numberOfDevices);
    save_files (activeRules, numberOfRules, activeDevices, numberOfDevices);
    time(&time_start);
    printf("Automation started at: %s\n", ctime (&time_start));

    /* Create the file io.txt, after which the simulator will start its process */
    start = fopen ("start", "w");
    fclose (start);

    /* Create io files */
    write_current_state (activeDevices, numberOfDevices);

    /* automation is checked in a loop: */
    automation_loop (activeRules, numberOfRules, activeDevices, numberOfDevices);
}

int main(int argc, char const *argv[])
{
    /* Initialize variables */
    FILE * rulesIn = fopen ("rules_test.txt", "r");
    FILE * devicesIn = fopen ("devices_test.txt", "r");
    int i,
        numberOfRules,
        numberOfDevices;
    rule *activeRules;
    device *activeDevices;

    /* If the file doesn't exist, then create the file. */
    if(rulesIn == NULL) 
    {
        FILE * tmp;
        printf ("Rule file doesn't exist, it's being created.\n");
        tmp = fopen ("rules_test.txt", "w");
        fclose (tmp);
        if(OS == 0){
            exit(1);
        }
    }

    /* If the file doesn't exist, then create the file. */
    if(devicesIn == NULL) 
    {
        FILE * tmp;
        printf ("Device file doesn't exist, it's being created.\n");
        tmp = fopen ("devices_test.txt", "w");
        fclose (tmp);
        if(OS == 0){
            exit(1);
        }
    }

    /* Allocate and zero initialze arrays of rules and devices */
    activeRules = (rule *) calloc (MAX_RULES, sizeof(rule));
    activeDevices = (device *) calloc (MAX_DEVICES, sizeof(device));

    /* Load existing rules from file */
    numberOfRules = load_rules (rulesIn, activeRules);
    fclose (rulesIn);

    /* Load existing devices from file */
    numberOfDevices = load_devices (devicesIn, activeDevices);
    fclose (devicesIn);

    /* Print loading info */
    printf ("Loaded %d rules, and %d devices.\n---------------------\n", numberOfRules, numberOfDevices);

    /* Allow to run i daemon mode, otherwise print menu */
    if(argc > 1 && strcmp(argv[0],"-d"))
    {
        automation_init (activeRules, numberOfRules, activeDevices, numberOfDevices);
    } else {
        /* Print menu */
        int option; 
        do {
            printf("Please choose an option.\n"
                   "  '-1' Quit\n"
                   "  '1' Print list of rules [%d]\n"
                   "  '2' Print list of devices [%d]\n"
                   "  '3' Add rule\n"
                   "  '4' Add device\n"
                   "  '5' Edit rule\n"
                   "  '6' Edit device\n"
                   "  '7' Delete rule\n"
                   "  '8' Delete device\n"
                   "  '9' Start home automation\n"
                   ">", numberOfRules, numberOfDevices);
            scanf("%d", (int *) &option);
            printf("---------------------\n");
            switch (option)
            {
                case -1: printf ("Bye\n"); break;
                case 1: for (i = 0; i < numberOfRules; i++) {print_single_rule(activeRules, i);} break;
                case 2: for (i = 0; i < numberOfDevices; i++) {print_single_device(activeDevices, i);} break;
                case 3: numberOfRules = add_rule (activeRules, numberOfRules); break;
                case 4: numberOfDevices = add_device (activeDevices, numberOfDevices); break;
                case 5: edit_rule (activeRules, numberOfRules); break;
                case 6:  break;
                case 7: numberOfRules = delete_rule (activeRules, numberOfRules); break;
                case 8: numberOfDevices = delete_device (activeDevices, numberOfDevices); break;
                case 9: automation_init (activeRules, numberOfRules, activeDevices, numberOfDevices); break;
                default: printf("Invalid option try again.\n"); break;
            }
            printf("---------------------\n");
        } while (option != -1);

    }

    save_files (activeRules, numberOfRules, activeDevices, numberOfDevices);
    write_current_state (activeDevices, numberOfDevices);

    remove("start");
    /* remove temporary files used for communication 
    remove("time.txt");

    for(i = 0; i < numberOfDevices; i++)
    {
        remove(activeDevices[i].id);
    } */

    return 0;
}

/* Outline for rules file format (1): 

AUTO_TURN_ON_LIGHT
1
1
480
22  32  -1  -1  -1  -1  -1  -1  -1  -1
45  -1  -1  -1  -1  -1  -1  -1  -1  -1 

*/

/* Outline for device file format (2): 

TEST_LAMPE_ENTRANCE
22

*/


/* Outline for time file format (3):

TIMESTAMP (IN MINUTES)

IE. 360 (== 06:00)

*/

/* Outline for io file format (4):

Filename is name of device, contents is state (0 or 1). 

*/