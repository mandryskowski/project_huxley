#!/bin/sh
# gcc -Wall -o predefinedRooms/structure_builder predefinedRooms/structure_builder.c game_math.c -lm
gcc -Wall -o predefinedRooms/room_generator predefinedRooms/structure_builder.c predefinedRooms/room_generator.c game_math.c -lm

./predefinedRooms/room_generator
make game
./game
