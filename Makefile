
GCC = gcc
#GCC = gcc

INCLUDE_LIB = -Ilib/include
SRC_LIB = lib/src/*.c


FLAGS = -Wall -Werror -Wpedantic -pedantic
OUTPUT = out.exe

DLL_FLAGS = -fPIC -shared  -Wl,--out-implib,libMemTrack.dll.a
DLL_OUTPUT = libMemTrack.dll

default: build run


build_lib:
	@${GCC} -O2 -march=native -flto lib/src/alloc.c lib/src/linked_list.c -o ${DLL_OUTPUT} -DDLL_EXPORTS ${DLL_FLAGS} ${INCLUDE_LIB} ${FLAGS}  

build: 
	@${GCC} ${SRC_LIB} -o ${OUTPUT} -DSTATIC_LINK  ${INCLUDE_LIB} ${FLAGS}  
#-DTRACK_ALLOCATIONS
run:
	@${OUTPUT}

