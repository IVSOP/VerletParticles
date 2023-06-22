#!/bin/bash

if make -j$(nproc) -C build; then
		printf "\n"
		./build/GAME
else
	printf ">> build failed\n"
fi
