#!/bin/sh

set -xe

CC=cc
CFLAGS="-Wall -Wextra -Wshadow -std=c11 -pedantic -ggdb"
LIBS=""

OUT=out

if [ ! -d "$OUT" ]; then
    mkdir -p "$OUT"
fi

$CC $CFLAGS -o $OUT/main main.c $LIBS
