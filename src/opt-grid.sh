#!/bin/bash

IFS=$'\n'
for param in $(grep -v -e "^#" grid.param)
do
    export row=$(echo $param | cut -d" " -f 1)
    export col=$(echo $param | cut -d" " -f 2)
    export d=$(echo $param | cut -d" " -f 3)
    export r=$(echo $param | cut -d" " -f 4)
    ngraphs=$(echo $param | cut -d" " -f 5)
    optflag=$(echo $param | cut -d" " -f 6)
    export dirname=n"$row"x"$col"d"$d"r"$r"
    if [ $optflag == opt ] ; then
        find $dirname | grep seed | xargs -L1 -P8 ./mini-opt-grid.sh
    fi
done
