#!/bin/sh
for i in {0..10}
do
    echo "##########################################################################"
    COTTON_WORKERS=4 turbostat --Joules ./Matrix.e $i 2000 2>&1 |tee output/o$i.out
    echo $i
    echo "###########################################################################"
    sleep 3s
done
