#!/bin/bash
infile=$1
outfile=$(echo $infile | sed s/seed/opt1/g)
logfile=$(echo $outfile | sed s/\.elist/\.log/g)
./opt-sa.out --size $row $col -r $r\
             --heat-loss 1e-5 -T 1.5\
             -i $infile -o $outfile --log $logfile\
             2> /dev/null
