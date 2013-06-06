#!/bin/sh

while :
do
    sleep 1
    stat=`ifstat -i re0 1 1 | tail -1`
    in=`echo $stat | awk '{print $1}'`
    out=`echo $stat | awk '{print $2}'`

    time=`date "+%H:%M"`
    new1="                    { \"title\" : \"$time\", \"value\" : $in},"
    new2="                    { \"title\" : \"$time\", \"value\" : $out},"
    awk 'NR != 10' liveNet.json > .liveNet.tmp && mv .liveNet.tmp liveNet.json
    awk -v n="$new1" 'NR == 310 {print n} {print}' liveNet.json > .liveNet.tmp && mv .liveNet.tmp liveNet.json
    awk 'NR != 316' liveNet.json > .liveNet.tmp && mv .liveNet.tmp liveNet.json
    awk -v n="$new2" 'NR == 616 {print n} {print}' liveNet.json > .liveNet.tmp && mv .liveNet.tmp liveNet.json
done
