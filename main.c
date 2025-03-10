#define WINVER 0x0500
#include <windows.h>
#include <stdio.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
    FILE *planning;
    typedef struct _entry {
	long start;
	long stop;
    } Entry;
#define MAX_PAUSES 20
    Entry schedule[MAX_PAUSES];
    int last = 0;

    planning = fopen("schedule.mj","r");

    if (planning) {
	    while(!feof(planning) && last < MAX_PAUSES) {
		    long h,m,d;

		    if (3 == fscanf(planning, "%ld:%ld %ld\n", &h, &m, &d)) {
				printf("Pause at %02d:%02d for %d minutes\n", h,m,d);
				fflush(stdout);

				schedule[last].start = h * 60 + m;
				schedule[last].stop = schedule[last].start + d;

				last++;
		    } else
		    if (2 == fscanf(planning, "START %ld:%ld\n", &h, &m)) {
				// Day shift start
				printf("Starting at %02d:%02d\n", h,m);
				fflush(stdout);

				schedule[last].start = 0;
				schedule[last].stop = h * 60 + m;

				last++;
		    } else
		    if (2 == fscanf(planning, "END %ld:%ld\n", &h, &m)) {
				// Day shift stop
				printf("Stopping at %02d:%02d\n", h,m);
				fflush(stdout);

				schedule[last].start = h * 60 + m;
				schedule[last].stop = 23*60+59;

				last++;
		    }
		    else {
				char dummy[255];
				fgets(dummy, sizeof(dummy), planning);
				printf("Skipping '%s'\n", dummy);
				fflush(stdout);
		    }
	    }

	    fclose(planning);
    } else {
	    MessageBox(NULL, "No pause??? Really?", "Michael Jakson", MB_ICONEXCLAMATION|MB_DEFAULT_DESKTOP_ONLY);
    }

    INPUT ip;

    memset(&ip, 0, sizeof(ip));
    ip.type = INPUT_MOUSE;
    ip.mi.dwFlags = MOUSEEVENTF_MOVE;
 
    LONG d = 1;
    int active = 1;

    printf("Starting MJ\n");
    fflush(stdout);

    while(1) {
	    SYSTEMTIME now;
	    long minutes;
#ifdef DEBUG
	    int was_active;
#endif

	    if (active) {
		    ip.mi.dx = d;
		    ip.mi.dy = d;
		    SendInput(1, &ip, sizeof(INPUT));
		    d = -d;
	    }

	    Sleep(1000);

	    GetLocalTime(&now);
	    minutes = now.wHour * 60 + now.wMinute;

#ifdef DEBUG
	    was_active = active;
#endif

	    active = 1;
	    for(int e = 0; e < last; e++) {
		    if (minutes >= schedule[e].start && minutes <= schedule[e].stop) {
			    active = 0;
			    break;
		    }
	    }

#ifdef DEBUG
	    if (active != was_active) {
		printf("%d %02d:%02d\n", active, now.wHour, now.wMinute);
	    }
#endif
    }
 
    return 0;
}
