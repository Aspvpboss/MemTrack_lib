
GCC = gcc


INCLUDE_LIB = -Ilib/include
SRC_LIB = lib/src/*.c


FLAGS = -Wall -Werror -Wpedantic -pedantic
OUTPUT = out.exe

DLL_FLAGS = -fPIC -shared  -Wl,--out-implib,libMemTrack.a
DLL_OUTPUT = libMemTrack.dll

default: build run

#-O2 -march=native
build_lib:
	@${GCC} -DDLL_EXPORTS  ${SRC_LIB} -o ${DLL_OUTPUT} ${DLL_FLAGS} ${INCLUDE_LIB} ${FLAGS}  

build: build_lib 
	@${GCC} main.c -o ${OUTPUT} ${INCLUDE_LIB} -lMemTrack -L. -DTRACK_ALLOCATIONS  ${FLAGS}  

run:
	@${OUTPUT}

