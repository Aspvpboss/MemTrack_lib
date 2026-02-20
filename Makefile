
GCC = gcc


INCLUDE_LIB = -IMemTrack
SRC_LIB = lib/src/*.c


FLAGS = -Wall -Werror -Wpedantic -pedantic
OUTPUT = build/out.exe

DLL_FLAGS = -fPIC -shared  -Wl,--out-implib,libMemTrack.a
DLL_OUTPUT = libMemTrack.dll

default: json build run

json:
	@compiledb -n -- make build

build:  
	@${GCC} main.c -o ${OUTPUT} ${INCLUDE_LIB} -DTRACK_ALLOCATIONS  ${FLAGS}  

run:
	@${OUTPUT}

