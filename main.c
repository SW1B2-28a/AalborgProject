/*
 * B2-28a
 * Version 1.0 - Initial Concept  
 * 24-11-2013
 *
 * This is the main part of the program.
 *
 * Outline for file formats located in the end of this file.
 * Variables i and j are used as counters in loops etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

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

void print_line_seperator (void)
{
    printf("\n---------------------\n");
}

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

        fprintf(rules, "\n");

        for (j = 0; j < DEVICES_PR_RULE; j++)
        {
            fprintf(rules, "%d ", activeRules[i].reactantsDisable[j]);
        }
        
        fprintf(rules, "\n\n");
    }
}

void print_single_rule (rule *activeRules, int ruleNumber, device *activeDevices) 
{
    int i, cnt = 0;
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
        if(activeRules[ruleNumber].dependencies[i] != -1)
            printf("    %d: %s\n", activeRules[ruleNumber].dependencies[i], 
                activeDevices[activeRules[ruleNumber].dependencies[i]].name);
        else
            cnt++;
    }

    if(cnt == 10)
        printf("    None\n");

    cnt = 0;

    printf("Affects devices (Enables):\n");
    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        if(activeRules[ruleNumber].reactantsEnable[i] != -1)
            printf("    %d: %s\n", activeRules[ruleNumber].reactantsEnable[i], 
                activeDevices[(activeRules[ruleNumber].reactantsEnable[i])].name);
        else
            cnt++;
    }

    if(cnt == 10)
        printf("    None\n");

    cnt = 0;

    printf("Affects devices (Disables):\n");
    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        if(activeRules[ruleNumber].reactantsDisable[i] != -1)
            printf("    %d: %s\n", activeRules[ruleNumber].reactantsDisable[i], 
                activeDevices[(activeRules[ruleNumber].reactantsDisable[i])].name);
        else
            cnt++;
    }

    if(cnt == 10)
        printf("    None\n");

    printf("\n");
}

void edit_actived_by (rule *activeRules, int ruleNumber, device *activeDevices, int numberOfDevices, int numberOfRules)
{
    int tmp, cnt = 0, i, j, devicesInRule[MAX_DEVICES];
    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        if (activeRules[ruleNumber].dependencies[i] != -1)
        {
            /* This gets the id of the device, not the position in array*/
            tmp = activeRules[ruleNumber].dependencies[i];
            for (j = 0; j < numberOfDevices; j++)
            {
                if (activeDevices[j].id == tmp)
                {
                    devicesInRule[cnt] = j;
                    cnt++;
                }
            }
        }
    }

    printf("This rule is actived by %d device%s.\n", cnt, cnt > 1 ? "s" : "");
    if (cnt > 0)
    {
        printf("The device%s:\n", cnt > 1 ? "s are" : " is");
        for (i = 0; i < cnt; i++)
        {
            printf("%d: %s\n", activeDevices[devicesInRule[i]].id, activeDevices[devicesInRule[i]].name);
        }
    }
    printf("Avalible devices is:\n");
    for (i = 0; i < numberOfDevices; i++)
    {
        printf("%d %s\n", activeDevices[i].id, activeDevices[i].name);
    }
    printf("Enter ALL devices (id) to be added seperated by newline (-1 to quit)\n>");
    cnt = 0;
    while (scanf("%d", &tmp) == 1 && cnt < DEVICES_PR_RULE)
    {
        activeRules[ruleNumber].dependencies[cnt] = tmp;
        if(tmp == -1)
        {
            break;
        }
        cnt++;
        printf(">");
    }

    for (i = cnt + 1; i < DEVICES_PR_RULE; i++)
    {
        activeRules[ruleNumber].dependencies[i] = -1;
    }
    printf("Rule saved.\n");
}

void edit_activates (rule *activeRules, int ruleNumber, device *activeDevices, int numberOfDevices, int numberOfRules)
{
    int tmp, cnt = 0, i, j, devicesInRule[MAX_DEVICES];
    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        if (activeRules[ruleNumber].reactantsEnable[i] != -1)
        {
            /* This gets the id of the device, not the position in array*/
            tmp = activeRules[ruleNumber].reactantsEnable[i];
            for (j = 0; j < numberOfDevices; j++)
            {
                if (activeDevices[j].id == tmp)
                {
                    devicesInRule[cnt] = j;
                    cnt++;
                }
            }
        }
    }

    printf("This rule actives %d device%s.\n", cnt, cnt > 1 ? "s" : "");
    if (cnt > 0)
    {
        printf("The devices are:\n");
        for (i = 0; i < cnt; i++)
        {
            printf("%d: %s\n", activeDevices[devicesInRule[i]].id, activeDevices[devicesInRule[i]].name);
        }
    }
    printf("Avalible devices is:\n");
    for (i = 0; i < numberOfDevices; i++)
    {
        printf("%d %s\n", activeDevices[i].id, activeDevices[i].name);
    }
    printf("Enter ALL devices (id) to be added seperated by newline (-1 to quit)\n>");
    cnt = 0;
    while (scanf("%d", &tmp) == 1 && cnt < DEVICES_PR_RULE)
    {
        activeRules[ruleNumber].reactantsEnable[cnt] = tmp;
        if(tmp == -1)
            break;
        cnt++;
        printf(">");
    }

    for (i = cnt + 1; i < DEVICES_PR_RULE; i++)
    {
        activeRules[ruleNumber].reactantsEnable[i] = -1;
    }
    printf("Rule saved.\n");
}

void edit_deactivates (rule *activeRules, int ruleNumber, device *activeDevices, int numberOfDevices, int numberOfRules)
{
    int tmp, cnt = 0, i, j, devicesInRule[MAX_DEVICES];
    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        if (activeRules[ruleNumber].reactantsDisable[i] != -1)
        {
            /* This gets the id of the device, not the position in array*/
            tmp = activeRules[ruleNumber].reactantsDisable[i];
            for (j = 0; j < numberOfDevices; j++)
            {
                if (activeDevices[j].id == tmp)
                {
                    devicesInRule[cnt] = j;
                    cnt++;
                }
            }
        }
    }

    printf("This rule deactives %d device%s.\n", cnt, cnt > 1 ? "s" : "");
    if (cnt > 0)
    {
        printf("The devices are:\n");
        for (i = 0; i < cnt; i++)
        {
            printf("%d: %s\n", activeDevices[devicesInRule[i]].id, activeDevices[devicesInRule[i]].name);
        }
    }
    printf("Avalible devices is:\n");
    for (i = 0; i < numberOfDevices; i++)
    {
        printf("%d %s\n", activeDevices[i].id, activeDevices[i].name);
    }
    printf("Enter ALL devices (id) to be added seperated by newline (-1 to quit)\n>");
    cnt = 0;
    while (scanf("%d", &tmp) == 1 && cnt < DEVICES_PR_RULE)
    {
        activeRules[ruleNumber].reactantsDisable[cnt] = tmp;
        if(tmp == -1)
            break;
        cnt++;
        printf(">");
    }

    for (i = cnt + 1; i < DEVICES_PR_RULE; i++)
    {
        activeRules[ruleNumber].reactantsDisable[i] = -1;
    }
    printf("Rule saved.\n");
}

void edit_time_settings (rule *activeRules, int ruleNumber, device *activeDevices)
{
    printf("Currently time activation is %sabled (At time %02d:%02d)\n"
           "Enter 1 if you want it enabled or 0 if not.\n"
           ">", 
            activeRules[ruleNumber].timerBased ? "en" : "dis", activeRules[ruleNumber].min / 60,
            activeRules[ruleNumber].min % 60);
    scanf("%d", &activeRules[ruleNumber].timerBased);
    if (activeRules[ruleNumber].timerBased) 
    {
        int hr, mn;
        printf("Set the time you want it to active at ( HH:MM IE. 08:05 )\n>");
        scanf("%d:%d", &hr, &mn);
        activeRules[ruleNumber].min = hr * 60 + mn;
    } else {
        activeRules[ruleNumber].min = 0;
    }
}

void edit_rule (rule *activeRules, int numberOfRules, device *activeDevices, int numberOfDevices)
{
    int ruleNumber, option, i;
    do {
        printf("Enter the number of the rule which is to be edited: (-1 to quit)\n");
        for (i = 0; i < numberOfRules; i++)
        {
            printf("    '%d': %s\n", i, activeRules[i].name);
        }
        printf(">");
        scanf("%d", &ruleNumber);
        if (ruleNumber >= numberOfRules || ruleNumber < - 1)
        {
            printf("Rule not found\n");
        }  else if ( ruleNumber == - 1) {
            /* void function return with no value to exit */
            return;
        }

    } while (ruleNumber >= numberOfRules || ruleNumber < - 1);

    print_line_seperator();
    print_single_rule (activeRules, ruleNumber, activeDevices);
    print_line_seperator();

    /* Prompt user for which action to take: */
    printf("Please select an action:\n"
           "    '1': Go Back to main menu.\n"
           "    '2': Go one step back and select a different rule.\n"
           "    '3': Edit rule name.\n"
           "    '4': Set time activation settings.\n"
           "    '5': Set device activation devices.\n"
           "    '6': Set which devices will be enabled.\n"
           "    '7': Set which devices will be disabled.\n"
           ">");

    scanf("%d", (int *) &option);
    print_line_seperator();
    switch (option)
    {
        case 1: 
            return; 
        case 2: 
            edit_rule (activeRules, numberOfRules, activeDevices, numberOfDevices); return;
        case 3: 
            printf("Enter a new name (Currently '%s'):\n>", activeRules[ruleNumber].name);
            scanf("%s", activeRules[ruleNumber].name);
            break;
        case 4:
            edit_time_settings (activeRules, ruleNumber, activeDevices);
            break;
        case 5:
            edit_actived_by (activeRules, ruleNumber, activeDevices, numberOfDevices, numberOfRules);
            break;
        case 6:
            edit_activates (activeRules, ruleNumber, activeDevices, numberOfDevices, numberOfRules);            
            break;
        case 7:
            edit_deactivates (activeRules, ruleNumber, activeDevices, numberOfDevices, numberOfRules);
            break;
        default: printf("Invalid option. (%d)\n", option); break;
    }
}

int add_rule (rule *activeRules, int numberOfRules, device *activeDevices, int numberOfDevices) 
{
    int i;

    printf("Enter a name for the rule (no spaces):\n>");
    scanf("%s", activeRules[numberOfRules].name);
    activeRules[numberOfRules].active = 1;
    
    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        activeRules[numberOfRules].dependencies[i] = -1;
        activeRules[numberOfRules].reactantsEnable[i] = -1;
        activeRules[numberOfRules].reactantsDisable[i] = -1;
    }
    print_line_seperator();
    edit_time_settings (activeRules, numberOfRules, activeDevices);
    print_line_seperator();
    edit_actived_by (activeRules, numberOfRules, activeDevices, numberOfDevices, numberOfRules);
    print_line_seperator();
    edit_activates (activeRules, numberOfRules, activeDevices, numberOfDevices, numberOfRules);
    print_line_seperator();            
    edit_deactivates (activeRules, numberOfRules, activeDevices, numberOfDevices, numberOfRules);
    print_line_seperator();

    return numberOfRules + 1;
}

int delete_rule (rule *activeRules, int numberOfRules)
{
    int ruleNumber, i;
    do {
        printf("Enter the number of the rule which is to be deleted: (-1 to quit)\n");
        for (i = 0; i < numberOfRules; i++)
        {
            printf("%d: %s\n", i, activeRules[i].name);
        }
        printf(">");
        scanf("%d", &ruleNumber);
        if (ruleNumber >= numberOfRules || ruleNumber < - 1)
        {
            printf("Rule not found\n");
        } else if ( ruleNumber == - 1) 
        {
            return numberOfRules;
        }

    } while (ruleNumber >= numberOfRules || ruleNumber < - 1);

    /* 
     * Deletion of a single array element.
     * Moves the elements of the next til the one deleted.
     * Repeat for the rest of the array.
     */
    for ( i = ruleNumber; i < numberOfRules - 1; i++ )
    {
        activeRules[i] = activeRules[i + 1];
    }

    return numberOfRules - 1;
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

    printf("Id:\t%d\n\n", activeDevices[deviceNumber].id);
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

    for ( i = option; i < numberOfDevices; i++ )
    {
        strcpy (activeDevices[i].name, activeDevices[i + 1].name);
        activeDevices[i].state = activeDevices[i + 1].state;
        activeDevices[i].id = activeDevices[i + 1].id;
    }

    return numberOfDevices - 1;
}

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
    int min, ready = 0;

    /* Using <sys/time.h> to get time */
    struct timeval tempTime;

    FILE * ftime;

    while (!ready)
    {
        gettimeofday(&tempTime, 0);
        if((int) tempTime.tv_sec % 2 == 0)
        {
            ftime = fopen ("time.txt", "r");
            if (ftime != NULL)
               ready = 1;
        }
        Sleep(250);
    }

    fscanf(ftime, "%d", &min);
    fclose(ftime);
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
    printf("<------------------------------------\n");
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

    /* Return 0 if the rule is based on time not state */
    if(aR[rN].timerBased == 1)
        return 0;

    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        if (aR[rN].dependencies[i] == -1)
        {
            depAchived++;
        } else {
            for (j = 0; j < NoD; j++)
            {
                if (aR[rN].dependencies[i] == aD[j].id && aD[j].state)
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

        /* Using <sys/time.h> to get time */
    struct timeval tempTime;

    while(min1 < 1440)
    {   
        gettimeofday(&tempTime, 0);
        /* If a check haven't occurred and the time is equal check */
        if(!runAlready && (int) tempTime.tv_sec % 2 == 0)
        {

            printf("Checking ... %02d:%02d\n", min1 / 60, min1 % 60);

            /* Read current io from files */
            load_current_state (activeDevices, numberOfDevices);

            /* Checks if any timerBased rules should trigger */
            min2 = get_current_time_in_minutes();
            for (i = 0; i < numberOfRules; i++)
            {
                if(activeRules[i].timerBased && (activeRules[i].min > min1 && activeRules[i].min <= min2))
                {
                    printf("%s triggered by time. (%d-%d)\n", activeRules[i].name, min1, min2);
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
        if ((int) tempTime.tv_sec % 2 == 1) {
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
    /* Using <sys/time.h> to get time */
    struct timeval tempTime;
    
    printf("Saving all rules [%d] and devices [%d]\n", numberOfRules, numberOfDevices);
    save_files (activeRules, numberOfRules, activeDevices, numberOfDevices);
    gettimeofday(&tempTime, 0);
    time_t currentTime = tempTime.tv_sec;
    printf("Automation started at: %s\n", ctime (&currentTime));

    /* Create io files */
    write_current_state (activeDevices, numberOfDevices);

    /* Create the file io.txt, after which the simulator will start its process */
    start = fopen ("start", "w");
    fclose (start);

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

    char filename[16];

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
        int option = 0; 
        do {
            printf("Please choose an option.\n"
                   "  '-1' Quit\n"
                   "  '1' Print list of rules [%d]\n"
                   "  '2' Print list of devices [%d]\n"
                   "  '3' Add rule\n"
                   "  '4' Add device\n"
                   "  '5' Edit rule\n"
                   "  '6' Delete rule\n"
                   "  '7' Delete device\n"
                   "  '8' Start home automation\n"
                   ">", numberOfRules, numberOfDevices);
            if(scanf("%d", (&option)) != 1)
            {
                /* If input isn't as expected (NaN) then clear stdin */
                printf("Invalid input.\n");
                fgetc(stdin);
            }
            print_line_seperator();
            switch (option)
            {
                case -1: printf ("Bye\n"); break;
                case 1: for (i = 0; i < numberOfRules; i++) {print_single_rule(activeRules, i, activeDevices);} break;
                case 2: for (i = 0; i < numberOfDevices; i++) {print_single_device(activeDevices, i);} break;
                case 3: numberOfRules = add_rule (activeRules, numberOfRules, activeDevices, numberOfDevices); break;
                case 4: numberOfDevices = add_device (activeDevices, numberOfDevices); break;
                case 5: edit_rule (activeRules, numberOfRules, activeDevices, numberOfDevices); break;
                case 6: numberOfRules = delete_rule (activeRules, numberOfRules); break;
                case 7: numberOfDevices = delete_device (activeDevices, numberOfDevices); break;
                case 8: automation_init (activeRules, numberOfRules, activeDevices, numberOfDevices); break;
                default: printf("Invalid option try again.\n"); break;
            }
            print_line_seperator();
        } while (option != -1);

    }

    save_files (activeRules, numberOfRules, activeDevices, numberOfDevices);
    write_current_state (activeDevices, numberOfDevices);

    
    /* remove temporary files used for communication */
    remove("start");
    remove("time.txt");
    for(i = 0; i < numberOfDevices; i++)
    {
        sprintf(filename, "%d", activeDevices[i].id);
        remove(filename);
    } 

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