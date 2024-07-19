#define WINVER 0x0500
#include <windows.h>
#include <stdio.h>

#ifdef DEBUG
int main(int argc, char **argv)
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
#endif
{
    FILE *planning;
    typedef struct _entry {
	SYSTEMTIME start;
	SYSTEMTIME stop;
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

			    GetLocalTime(&schedule[last].start);
			    schedule[last].start.wHour = (WORD)h,
			    schedule[last].start.wMinute = (WORD)m,
			    schedule[last].start.wSecond = 0;
			    schedule[last].start.wMilliseconds = 0;

			    m += d;
			    h += (m/60);
			    m %= 60;

			    GetLocalTime(&schedule[last].stop);
			    schedule[last].stop.wHour = (WORD)h,
			    schedule[last].stop.wMinute = (WORD)m,
			    schedule[last].stop.wSecond = 0;
			    schedule[last].stop.wMilliseconds = 0;

			    last++;
		    } else
		    if (2 == fscanf(planning, "START %ld:%ld\n", &h, &m)) {
			// Day shift start
			printf("Starting at %02d:%02d\n", h,m);
			fflush(stdout);

			GetLocalTime(&schedule[last].start);
			schedule[last].start.wHour = 0;
			schedule[last].start.wMinute = 0;
			schedule[last].start.wSecond = 0;
			schedule[last].start.wMilliseconds = 0;

			GetLocalTime(&schedule[last].stop);
			schedule[last].stop.wHour = (WORD)h,
			schedule[last].stop.wMinute = (WORD)m,
			schedule[last].stop.wSecond = 0;
			schedule[last].stop.wMilliseconds = 0;

			last++;
		    } else
		    if (2 == fscanf(planning, "END %ld:%ld\n", &h, &m)) {
			// Day shift stop
			printf("Stopping at %02d:%02d\n", h,m);
			fflush(stdout);

			GetLocalTime(&schedule[last].start);
			schedule[last].start.wHour = (WORD)h,
			schedule[last].start.wMinute = (WORD)m,
			schedule[last].start.wSecond = 0;
			schedule[last].start.wMilliseconds = 0;

			GetLocalTime(&schedule[last].stop);
			schedule[last].stop.wHour = 23;
			schedule[last].stop.wMinute = 59;
			schedule[last].stop.wSecond = 59;
			schedule[last].stop.wMilliseconds = 999;

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
	    MessageBox(NULL, "No pause!!!", "Michael Jakson", MB_ICONEXCLAMATION);
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

#ifdef DEBUG
	    was_active = active;
#endif

	    active = 1;
	    for(int e = 0; e < last; e++) {
		    if (now.wHour >= schedule[e].start.wHour
		     && now.wMinute >= schedule[e].start.wMinute
		     && now.wHour <= schedule[e].stop.wHour
		     && now.wMinute < schedule[e].stop.wMinute) {
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
