#!/bin/sh

totalIn=0.0
totalOut=0.0
for i in {1..60}
do
    sleep 1
    stat=`ifstat -i re0 1 1 | tail -1`
    in=`echo $stat | awk '{print $1}'`
    out=`echo $stat | awk '{print $2}'`

    totalIn=`echo "$totalIn + $in" | bc`
    totalOut=`echo "$totalOut + $out" | bc`

    time=`date "+%H:%M"`
    new1="                    { \"title\" : \"$time\", \"value\" : $in},"
    new2="                    { \"title\" : \"$time\", \"value\" : $out},"
    awk 'NR != 11' liveNet.json > .liveNet.tmp && mv .liveNet.tmp liveNet.json
    awk -v n="$new1" 'NR == 71 {print n} {print}' liveNet.json > .liveNet.tmp && mv .liveNet.tmp liveNet.json
    awk 'NR != 78' liveNet.json > .liveNet.tmp && mv .liveNet.tmp liveNet.json
    awk -v n="$new2" 'NR == 138 {print n} {print}' liveNet.json > .liveNet.tmp && mv .liveNet.tmp liveNet.json
done

totin=`echo "                    { \"title\" : \"$time\", \"value\" : $totalIn}," | sed "s/ \./ 0./g"`
totout=`echo "                    { \"title\" : \"$time\", \"value\" : $totalOut}," | sed "s/ \./ 0\./g" `
awk 'NR != 11' netTraf.json > .netTraf.tmp && mv .netTraf.tmp netTraf.json
awk -v n="$totin" 'NR == 34 {print n} {print}' netTraf.json > .netTraf.tmp && mv .netTraf.tmp netTraf.json
awk 'NR != 41' netTraf.json > .netTraf.tmp && mv .netTraf.tmp netTraf.json
awk -v n="$totout" 'NR == 64 {print n} {print}' netTraf.json > .netTraf.tmp && mv .netTraf.tmp netTraf.json
