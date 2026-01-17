
GCC = gcc


INCLUDE_LIB = -IMemTrack
SRC_LIB = lib/src/*.c


FLAGS = -Wall -Werror -Wpedantic -pedantic
OUTPUT = out.exe



default: json build run

json:
	@compiledb -n -- make build

build:  
	@${GCC} main.c -o ${OUTPUT} ${INCLUDE_LIB} -DTRACK_ALLOCATIONS  ${FLAGS}  

run:
	@${OUTPUT}

