# Michael-Jackson

Is a simple and VERY light Windows executable that acts as mouse jiggler.

Companion file is [**schedule.mj**](schedule.mj) which is one per line pause definition file.
You can still move mouse while it's running, move is invisible (almost).
During defined pause time, it simply suspends jiggling. This will let Windows
claim you're not AFK.

If no schedule.mj file is present, MJ won't pause at all, permanently jiggling.

Each line is composed of time of pause and duration of pause formatted as:
```
HH:MM d
```
Where HH is hours, MM minutes and d pause duration expressed in minutes.

You can define a day shift with the following lines:

```
START HH:MM
END HH:MM
```

This allows to stop jiggling when you're off the clock.

There is no UI, you must kill it using the TaskManager or using Powershell command:

```pwsh
PS C:\> Stop-Process -Name MJ
```

To compile under cigwin with MinGW64 and make packages installed, just type make.
There is a special debug target that creates an executable that prints out verbose information for ongoing jiggling.
