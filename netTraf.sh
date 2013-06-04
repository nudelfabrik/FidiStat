#!/bin/sh

stat=`ifstat -i re0 1 1 | tail -1`
in=`awk '{print $1}' $stat`
out=`awk '{print $2}' $stat`

time=`date "+%H:%M"`
new1="                    { \"title\" : \"$time\", \"value\" : $in},"
new2="                    { \"title\" : \"$time\", \"value\" : $out},"
awk 'NR != 9' load.json > .load.tmp && mv .load.tmp load.json
awk -v n="$new1" 'NR == 32 {print n} {print}' load.json > .load.tmp && mv .load.tmp load.json
awk 'NR != 38' load.json > .load.tmp && mv .load.tmp load.json
awk -v n="$new2" 'NR == 61 {print n} {print}' load.json > .load.tmp && mv .load.tmp load.json
