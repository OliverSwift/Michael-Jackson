mj: main.c
	x86_64-w64-mingw32-gcc -o $@.exe main.c -mwindows
	strip $@.exe
