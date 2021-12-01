#!/bin/sh

set -xe

CC=cc
CFLAGS="-Wall -Wextra -Wshadow -std=c11 -pedantic -ggdb"
LIBS=""

$CC $CFLAGS -o out/main main.c $LIBS
