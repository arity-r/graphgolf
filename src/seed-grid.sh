#!/bin/bash

export elemrow=4
export elemcol=16
IFS=$'\n'
for param in $(grep -v -e "^#" grid.param)
do
    export row=$(echo $param | cut -d" " -f 1)
    export col=$(echo $param | cut -d" " -f 2)
    export d=$(echo $param | cut -d" " -f 3)
    export r=$(echo $param | cut -d" " -f 4)
    ngraphs=$(echo $param | cut -d" " -f 5)
    export dirname=n"$row"x"$col"d"$d"r"$r"
    mkdir -p $dirname
    export stacknum=$(echo "$row/$elemrow*$col/$elemcol" | bc)
    if [ $stacknum == 1 ] ; then
        seq -w $ngraphs |\
            xargs -P8 -I{} ./rrg-bag.py --size $row $col -d$d -r$r\
                  -o "$dirname/seed-{}.elist"
    else
        export elemdir=n"$elemrow"x"$elemcol"d"$d"r"$r"
        seq -w $ngraphs | xargs --max-args=1 -P8 bash -E stitch-grid-mini.sh
    fi
done
