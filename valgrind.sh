#!/bin/bash

DIR="debug"

if cmake -DCMAKE_BUILD_TYPE=Debug -DGLFW_BUILD_DOCS=OFF -S . -B $DIR; then
	printf "\n"
	if make -j$(nproc) -C $DIR; then
		printf "\n"
		valgrind --tool=memcheck --leak-check=full --show-reachable=yes --num-callers=20\
				--track-fds=yes --track-origins=yes --error-exitcode=1 ./$DIR/GAME
	else
		printf ">> build failed\n"
	fi
else
	printf ">> configure failed\n"
fi
