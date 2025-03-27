#define WINVER 0x0500
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int last = 0;

typedef struct _entry {
    long start;
    long stop;
} Entry;

#define MAX_PAUSES 20
Entry schedule[MAX_PAUSES];

int read_schedule(const char *filename) {
    FILE *planning;

    planning = fopen(filename,"r");

    if (planning == NULL) return -1;

    srand( (unsigned)time( NULL ) );

    last = 0;

    while(!feof(planning) && last < MAX_PAUSES) {
        long h,m,d;
        char line[255];

        if ( NULL == fgets(line, sizeof(line), planning)) break;

        if (3 == sscanf(line, "%ld:%ld %ld\n", &h, &m, &d)) {
            printf("Pause at %02d:%02d for %d minutes\n", h,m,d);
            fflush(stdout);

            schedule[last].start = h * 60 + m;
            schedule[last].stop = schedule[last].start + d;

            schedule[last].start -= rand() & 3; // Up to 3 minutes earlier
            schedule[last].stop += rand() & 7;  // Up to 7 minutes later

            last++;
        } else
        if (2 == sscanf(line, "START %ld:%ld\n", &h, &m)) {
            // Day shift start
            printf("Starting at %02d:%02d\n", h,m);
            fflush(stdout);

            schedule[last].start = 0;
            schedule[last].stop = h * 60 + m;

            last++;
        } else
        if (2 == sscanf(line, "END %ld:%ld\n", &h, &m)) {
            // Day shift stop
            printf("Stopping at %02d:%02d\n", h,m);
            fflush(stdout);

            schedule[last].start = h * 60 + m;
            schedule[last].stop = 23*60+59;

            last++;
        }
        else {
            printf("Skipping '%s'\n", line);
            fflush(stdout);
        }
    }

    fclose(planning);

    return 0; // No error
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pCmdLine, int nCmdShow)
{
    if (read_schedule("schedule.mj") != 0) {
        MessageBox(NULL, "No pause??? Really?", "Michael Jackson", MB_ICONEXCLAMATION|MB_DEFAULT_DESKTOP_ONLY);
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
        int was_active;

        if (active) {
            ip.mi.dx = d;
            ip.mi.dy = d;
            SendInput(1, &ip, sizeof(INPUT));
            d = -d;
        }

        Sleep(10000);

        GetLocalTime(&now);
        minutes = now.wHour * 60 + now.wMinute;

        was_active = active;

        active = 1;
        for(int e = 0; e < last; e++) {
            if (minutes >= schedule[e].start && minutes <= schedule[e].stop) {
                active = 0;
                if (e == last - 1) {
                    // End of the day
                    read_schedule("schedule.mj"); // Reread and compute new time table
                }
                break;
            }
        }

        if (active != was_active) {
            FILE *report;

            report = fopen("status.txt","w");
            if (report) {
                fprintf(report, "%s since %02d:%02d\n", active?"On ":"Off", now.wHour, now.wMinute);
#ifdef DEBUG
                printf("%s %02d:%02d\n", active?"On ":"Off", now.wHour, now.wMinute);
#endif
                fclose(report);
            }
        }
    }

    return 0;
}
