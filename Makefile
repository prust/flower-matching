flowersmake:
ifeq ($(OS),Windows_NT)
	gcc -o bin/flowers.exe flowers.c -I /c/msys64/usr/lib/sdl2/x86_64-w64-mingw32/include/SDL2 -L /c/msys64/usr/lib/sdl2/x86_64-w64-mingw32/lib -lmingw32 -lSDL2main -lSDL2
else
	gcc -o bin/flowers flowers.c -L/usr/local/lib -I bin/SDL2.framework/Headers -F bin -framework SDL2
endif

flowersdebug:
	gcc -g -o bin/flowers flowers.c -L/usr/local/lib -I bin/SDL2.framework/Headers -F bin -framework SDL2