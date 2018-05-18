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
    if [ -d $dirname ] ; then
        ./score.py --size $row $col\
                   -o "$dirname/score.$(date -I | sed s/-//g).csv"\
                   -i $(find $dirname | grep elist)
    fi
done
