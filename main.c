/*
 * B2-28a
 * Version 0.1 - Initial idea
 * 24-11-2013
 *
 * Outline for fileformats located in the end of this file.
 */

#include <stdio.h>
#include <stdlib.h>

#define MAX_RULES 		100
#define MAX_DEVICES		100
#define DEVICES_PR_RULE 10

typedef struct {
	char name[50];
	int dependencies[DEVICES_PR_RULE],		/* devices (id) on which the rule depend upon  	*/
		reactants[DEVICES_PR_RULE];			/* devices (id) on which the rule affects		*/
} rule;

typedef struct {
	char name[50];
	int id,
		state;
} device;

int load_rules (FILE *rules, rule *activeRules)
{
	char ch;	/* Char to check if the next char is EOF 	*/
	int n = 0, 	/* Count number of rules loaded 		 	*/
		i;

	/* While the next char isn't EOF read rules from file 	*/
	while (((ch = fgetc(rules)) != EOF) && ungetc(ch, rules)) 
	{
		fscanf(rules, " %s", activeRules[n].name);

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

int add_rule (rule *activeRules, int numberOfRules) 
{
	printf("Navn paa regel:\n>");
	scanf("%s", activeRules[numberOfRules].name);

	int i;
	for (i = 0; i < DEVICES_PR_RULE; i++)
	{
		activeRules[numberOfRules].dependencies[i] = -1;
		activeRules[numberOfRules].reactants[i] = -1;
	}

	return numberOfRules + 1;
}

int load_devices (FILE *devices, device *activeDevices)
{
	char ch;	/* Char to check if the next char is EOF 	*/
	int n = 0; 	/* Count number of devices loaded 		 	*/

	/* While the next char isn't EOF read devices from file 	*/
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

int add_device (device *activeDevices, int numberOfDevices) {
	printf ("Navn paa enhed:\n>");
	scanf ("%s", activeDevices[numberOfDevices].name);
	printf ("Stadie [0/1]\n>");
	scanf ("%d", &activeDevices[numberOfDevices].state);

	activeDevices[numberOfDevices].id = numberOfDevices + 1;

	return numberOfDevices + 1;
}

int main(int argc, char const *argv[])
{
	FILE * rulesIn = fopen ("rules_test.txt", "r");
	FILE * devicesIn = fopen ("devices_test.txt", "r");

	/* If the file doesn't exist, then create the file. */
	if(rulesIn == NULL) 
	{
		fclose (rulesIn);
		printf ("Regl filen er ikke ekstisterende. Den skabes.\n");
		FILE * tmp = fopen ("rules_test.txt", "w");
		fclose (tmp);
		exit (1);
	}

	/* If the file doesn't exist, then create the file. */
	if(devicesIn == NULL) 
	{
		fclose (devicesIn);
		printf ("Enheds filen er ikke ekstisterende. Den skabes.\n");
		FILE * tmp = fopen ("devices_test.txt", "w");
		fclose (tmp);
		exit (1);
	}

	/* Initialise arrays of rules and devices */
	rule *activeRules = (rule *) calloc (MAX_RULES, sizeof(rule));
	device *activeDevices = (device *) calloc (MAX_DEVICES, sizeof(device));

	/* Load existing rules from file */
	int numberOfRules = load_rules (rulesIn, activeRules);
	fclose (rulesIn);

	/* Load existing devices from file */
	int numberOfDevices = load_devices (devicesIn, activeDevices);
	fclose (devicesIn);

	/* Add a rule and a device */
	numberOfDevices = add_device (activeDevices, numberOfDevices);
	numberOfRules = add_rule (activeRules, numberOfRules);

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
22 	32 	-1 	-1 	-1 	-1 	-1 	-1 	-1 	-1
45 	-1 	-1 	-1 	-1 	-1 	-1 	-1 	-1 	-1 

*/

/* Outline for rules file format (2) devices: 

TEST_LAMPE_ENTRANCE
22
1

*/
