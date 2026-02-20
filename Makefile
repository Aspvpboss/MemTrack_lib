
GCC = gcc

INCLUDE_LIB_TEST = -Ilib/include
INCLUDE_LIB = -IMemTrack
SRC_LIB = lib/src/*.c


FLAGS = -Wall -Werror -Wpedantic
OUTPUT = build/out.exe


default: build_test run


build_test:  
	@${GCC} main.c lib/src/memtrack.c -o ${OUTPUT} ${INCLUDE_LIB_TEST} -DTRACK_ALLOCATIONS  ${FLAGS}  

build:  
	@${GCC} main.c -o ${OUTPUT} ${INCLUDE_LIB} -DTRACK_ALLOCATIONS  ${FLAGS}  

run:
	@${OUTPUT}

