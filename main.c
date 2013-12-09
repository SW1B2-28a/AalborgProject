/*
 * B2-28a
 * Version 0.1 - Initial idea
 * 24-11-2013
 *
 * Outline for fileformats located in the end of this file.
 * Variables i and j are used as counters in loops etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_RULES       100
#define MAX_DEVICES     100
#define DEVICES_PR_RULE 10

typedef struct {
    char name[50];
    int dependencies[DEVICES_PR_RULE],      /* devices (id) on which the rule depend upon   */
        reactants[DEVICES_PR_RULE],         /* devices (id) on which the rule affects       */
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
            fscanf (rules, " %d ", &activeRules[n].reactants[i]);
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
            fprintf(rules, "%d ", activeRules[i].reactants[j]);
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
    
    printf("\nAffects devices:\n");

    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        printf("%d ", activeRules[ruleNumber].reactants[i]);
    }
    
    printf("\n\n");
}

int add_rule (rule *activeRules, int numberOfRules) 
{
    printf("Navn paa regel:\n>");
    scanf("%s", activeRules[numberOfRules].name);
    activeRules[numberOfRules].active = 1;
    activeRules[numberOfRules].timerBased = 1;
    activeRules[numberOfRules].min = 485;

    int i;
    for (i = 0; i < DEVICES_PR_RULE; i++)
    {
        activeRules[numberOfRules].dependencies[i] = -1;
        activeRules[numberOfRules].reactants[i] = -1;
    }

    return numberOfRules + 1;
}

int delete_rule (rule *activeRules, int numberOfRules)
{
    int ruleNumber;
    do {
        printf("Enter the number of the rule which is to be delted: (-1 to quit)\n>");
        scanf("%d", &ruleNumber);
        if (ruleNumber > numberOfRules - 1 || ruleNumber < - 1)
        {
            printf("Rule not found\n");
        } 
            else if ( ruleNumber == - 1) 
        {
            return numberOfRules;
        }

    } while (ruleNumber > numberOfRules - 1 || ruleNumber < - 1);

    /* 
     * Deletion of a single array element is the biggest disadvantage of arrays ... 
     * Replaces the current element of each in the struct of the next.
     * Looped to the end of the array.
     */

    int i, j;
    for ( i = ruleNumber; i < numberOfRules - 1; i++ )
    {
        strcpy (activeRules[i].name, activeRules[i + 1].name);
        activeRules[i].active = activeRules[i + 1].active;
        activeRules[i].timerBased = activeRules[i + 1].timerBased;
        activeRules[i].min = activeRules[i + 1].min;

        for (j = 0; j < DEVICES_PR_RULE; j++)
        {
            activeRules[i].dependencies[j] = activeRules[i + 1].dependencies[j];
            activeRules[i].reactants[j] = activeRules[i + 1].dependencies[j];
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
        
        fscanf(devices, "%d ", &activeDevices[n].state);

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
        
        fprintf(devices, "%d\n\n", activeDevices[i].state);

    }
}

void print_devices (device *activeDevices, int numberOfDevices) 
{
    int i;

    for (i = 0; i < numberOfDevices; i++)
    {
        printf("Name:\t%s\n", activeDevices[i].name);
        
        printf("Id:\t%d\n", activeDevices[i].id);
        
        printf("State:\t%d\n\n", activeDevices[i].state);

    }
}

int add_device (device *activeDevices, int numberOfDevices) 
{
    printf ("Enter a name for the device:\n>");
    scanf ("%s", activeDevices[numberOfDevices].name);
    printf ("State [0/1] (Defaults to 1 if invalid)\n>");
    scanf ("%d", &activeDevices[numberOfDevices].state);
    if(activeDevices[numberOfDevices].state != 0 || activeDevices[numberOfDevices].state != 1)
        activeDevices[numberOfDevices].state = 1;

    if (numberOfDevices > 0) 
        activeDevices[numberOfDevices].id = activeDevices[numberOfDevices - 1].id + 1;
    else
        activeDevices[numberOfDevices].id = numberOfDevices + 1;

    return numberOfDevices + 1;
}

int delete_device (device *activeDevices, int numberOfDevices)
{
    int deviceNumber;
    do {
        printf("Enter the id of the device which is to be delted: (-1 to quit)\n>");
        scanf("%d", &deviceNumber);
        if (deviceNumber > numberOfDevices - 1 || deviceNumber < - 1)
        {
            printf("Rule not found\n");
        } 
            else if ( deviceNumber == - 1) 
        {
            return numberOfDevices;
        }

    } while (deviceNumber > numberOfDevices - 1 || deviceNumber < - 1);

    int i;
    for ( i = deviceNumber; i < numberOfDevices - 1; i++ )
    {
        strcpy (activeDevices[i].name, activeDevices[i].name);
        activeDevices[i].state = activeDevices[i + 1].state;
        activeDevices[i].id = activeDevices[i + 1].id;
    }

    return numberOfDevices - 1;
}

int main(int argc, char const *argv[])
{
    FILE * rulesIn = fopen ("rules_test.txt", "r");
    FILE * devicesIn = fopen ("devices_test.txt", "r");

    /* If the file doesn't exist, then create the file. */
    if(rulesIn == NULL) 
    {
        printf ("Regl filen er ikke ekstisterende. Den skabes.\n");
        FILE * tmp = fopen ("rules_test.txt", "w");
        fclose (tmp);
        exit (1);
    }

    /* If the file doesn't exist, then create the file. */
    if(devicesIn == NULL) 
    {
        printf ("Enheds filen er ikke ekstisterende. Den skabes.\n");
        FILE * tmp = fopen ("devices_test.txt", "w");
        fclose (tmp);
        exit (1);
    }

    int i;

    /* Initialise arrays of rules and devices */
    rule *activeRules = (rule *) calloc (MAX_RULES, sizeof(rule));
    device *activeDevices = (device *) calloc (MAX_DEVICES, sizeof(device));

    /* Load existing rules from file */
    int numberOfRules = load_rules (rulesIn, activeRules);
    fclose (rulesIn);

    /* Load existing devices from file */
    int numberOfDevices = load_devices (devicesIn, activeDevices);
    fclose (devicesIn);

    /* Print loading info */
    printf ("Loaded %d rules, and %d devices.\n---------------------\n", numberOfRules, numberOfDevices);

    /* Print menu */
    int option; 
    do {
        printf("Please choose an option.\n"
               "  '0' Quit\n"
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
        scanf("%d", &option);
        printf("---------------------\n");
        switch (option)
        {
            case 0: printf ("Bye\n"); break;
            case 1: for (i = 0; i < numberOfRules; i++) {print_single_rule(activeRules, i);} break;
            case 2: print_devices (activeDevices, numberOfDevices); break;
            case 3: numberOfRules = add_rule (activeRules, numberOfRules); break;
            case 4: numberOfDevices = add_device (activeDevices, numberOfDevices); break;
            case 5: edit_rule (activeRules, numberOfRules); break;
            case 6:  break;
            case 7: numberOfRules = delete_rule (activeRules, numberOfRules); break;
            case 8: numberOfDevices = delete_device (activeDevices, numberOfDevices); break;
            case 9:  break;
            default: printf("Invalid option try again.\n"); break;
        }
        printf("---------------------\n");
    } while (option != 0);


    /* Open file in write mode and write all rules */
    FILE * rulesOut = fopen ("rules_test.txt", "w");
    write_rules (rulesOut, activeRules, numberOfRules);
    printf ("Rules written (%d)\n", numberOfRules);
    fclose (rulesOut);

    /* Open file in write mode and write all devices */
    FILE * devicesOut = fopen ("devices_test.txt", "w");
    write_devices (devicesOut, activeDevices, numberOfDevices);
    printf ("Devices written (%d)\n", numberOfDevices);
    fclose (devicesOut); 

    return 0;
}

/* Outline for rules file format (1) rules: 

AUTO_TURN_ON_LIGHT
1
1
480
22  32  -1  -1  -1  -1  -1  -1  -1  -1
45  -1  -1  -1  -1  -1  -1  -1  -1  -1 

*/

/* Outline for rules file format (2) devices: 

TEST_LAMPE_ENTRANCE
22
1

*/
