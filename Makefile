
GCC = gcc


INCLUDE_LIB = -IMemTrack
SRC_LIB = lib/src/*.c


FLAGS = -Wall -Werror -Wpedantic
OUTPUT = ./build/out_c.exe



default: json build run


json:
	@compiledb -n -- make build

build:  
	@${GCC} main.c -o ${OUTPUT} ${INCLUDE_LIB} -DTRACK_ALLOCATIONS  ${FLAGS}  

run:
	@${OUTPUT}




G++ = g++



cpp_all: json build_cpp run_cpp

