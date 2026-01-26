
GCC = gcc


INCLUDE_LIB = -IMemTrack
SRC_LIB = lib/src/*.c


FLAGS = -Wall -Werror -Wpedantic
OUTPUT = ./build/out_c.exe



default: c_all cpp_all 

c_all: json build run

json:
	@compiledb -n -- make build

build:  
	@${GCC} main.c -o ${OUTPUT} ${INCLUDE_LIB} -DTRACK_ALLOCATIONS  ${FLAGS}  

run:
	@${OUTPUT}




G++ = g++
OUTPUT_CPP = build/out_cpp.exe


cpp_all: json build_cpp run_cpp


build_cpp:
	@${G++} main.cpp -o ${OUTPUT_CPP} ${INCLUDE_LIB} -DTRACK_ALLOCATIONS ${FLAGS}

run_cpp:
	@${OUTPUT_CPP}

