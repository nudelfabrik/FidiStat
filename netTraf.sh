#!/bin/sh

stat=`ifstat -i re0 1 1 | tail -1`
in=`echo $stat | awk '{print $1}'`
out=`echo $stat | awk '{print $2}'`

time=`date "+%H:%M"`
new1="                    { \"title\" : \"$time\", \"value\" : $in},"
new2="                    { \"title\" : \"$time\", \"value\" : $out},"
awk 'NR != 10' netTraf.json > .netTraf.tmp && mv .netTraf.tmp netTraf.json
awk -v n="$new1" 'NR == 33 {print n} {print}' netTraf.json > .netTraf.tmp && mv .netTraf.tmp netTraf.json
awk 'NR != 39' netTraf.json > .netTraf.tmp && mv .netTraf.tmp netTraf.json
awk -v n="$new2" 'NR == 62 {print n} {print}' netTraf.json > .netTraf.tmp && mv .netTraf.tmp netTraf.json
