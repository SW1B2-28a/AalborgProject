#include <stdio.h>
#include <stdlib.h>
#include <time.h> 

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
#define OS                  1
#include <windows.h> 
#else
#define OS                  0
#include <unistd.h>
int Sleep(int sleepMs) {return sleep(sleepMs / 1000);}
#endif

int main (void)
{
	int min = 460,
		runAlready = 0,
		ready = 0;


	FILE * ftime;
	FILE * start;

    /* Wait for main to be ready */
	while (!ready)
	{
		start = fopen ("start", "r");
		if (start != NULL)
		{
			ready = 1;
		}

        printf("Fail\n");
        Sleep(1000);
	}

	while (min < 520)
	{
		if(!runAlready && (int) time(NULL) % 2 == 1)
		{
			ftime = fopen ("time.txt","w");
			fprintf (ftime, "%d\n", min);
			fclose (ftime);
			printf("Time: %02d:%02d (%d) adding 5 minutes!\n", min / 60, min % 60, min);
			runAlready = 1;
			min += 5;
		}

		if((int) time(NULL) % 2 == 0)
		{
			runAlready = 0;
		}

		/* If windows use sleep */
        if(OS)
        {
            Sleep(50);
        }
		
	}

	return 0;
}