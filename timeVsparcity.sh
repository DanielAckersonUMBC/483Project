#!/bin/bash
mydir=$PWD
mycsv="timeVsparcity.csv"

# usage: ./timeVsparcity.sh numTest pct_zero

if [[ ! -f $mycsv ]]; then
    echo "time,iterations,norm,size,nodes,threshold" > $mycsv
fi

#performn visualization
test=0;
while (($test <= $1-1)); do
    echo "test $test"
    ./matrix_writer 1 1000 $2
    ./matrix_reader -n 4 -i 1000000 -f Matrices -t 1e-4 | tee -a "$mycsv"
    let test=$test+1
done
