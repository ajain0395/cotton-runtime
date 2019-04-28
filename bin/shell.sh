#!/bin/sh
for i in {1..200}
do
    COTTON_WORKERS=4 ; ./NQueens.e 12
    echo $i
done
