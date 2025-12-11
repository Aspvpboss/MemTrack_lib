
GCC = gcc


INCLUDE_LIB = -Ilib/include
SRC_LIB = lib/src/*.c


FLAGS = -Wall -Werror -Wpedantic -pedantic
OUTPUT = out.exe


DLL_FLAGS = -fPIC -shared  -Wl,--out-implib,libMemTrack.a
DLL_OUTPUT = libMemTrack.dll

default: build run




build_lib:
<<<<<<< HEAD
	@${GCC} -DDLL_EXPORTS -O2 ${SRC_LIB} -o ${DLL_OUTPUT} ${DLL_FLAGS} ${INCLUDE_LIB} ${FLAGS}  
=======
	@${GCC} -O2 -DDLL_EXPORTS  ${SRC_LIB} -o ${DLL_OUTPUT} ${DLL_FLAGS} ${INCLUDE_LIB} ${FLAGS}  
>>>>>>> 439c4dde79a315996604353660055425a1dd1401

build: build_lib 
	@${GCC} main.c -o ${OUTPUT} ${INCLUDE_LIB} -lMemTrack -L. -DTRACK_ALLOCATIONS  ${FLAGS}  

run:
	@${OUTPUT}

