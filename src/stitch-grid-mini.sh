#!/bin/bash
infiles=$(find $elemdir | grep seed | sort -R | head -n $stacknum)
./stitch-grid.py\
    --osize "${row}x${col}" -d$d -r$r\
    --ofile "$dirname/seed-${1}.elist"\
    --isize $(seq $stacknum | xargs -I{} echo "${elemrow}x${elemcol}")\
    --ifiles $infiles
