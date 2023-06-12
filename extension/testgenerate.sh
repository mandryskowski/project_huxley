#!/bin/sh
gcc -Wall -o predefinedRooms/room_generator predefinedRooms/room_generator.c game_math.c -lm

./predefinedRooms/room_generator
make game
./game
