PROJECT = handle_finder

CCX64  := x86_64-w64-mingw32-gcc
CCX86  := i686-w64-mingw32-gcc

CFLAGS := -Wall -Werror -Os -s -fPIC

.DEFAULT: all
.PHONY: all
all: bof

.PHONY: bof
bof: $(PROJECT).x64.o $(PROJECT).x86.o

$(PROJECT).x64.o: src/main.c
	$(CCX64) -c $< -o dist/$@ $(CFLAGS) -DBOF

$(PROJECT).x86.o: src/main.c
	$(CCX86) -c $< -o dist/$@ $(CFLAGS) -DBOF

.PHONY: clean
clean:
	rm -f dist/$(PROJECT)x64.o
	rm -f dist/$(PROJECT)x86.o

