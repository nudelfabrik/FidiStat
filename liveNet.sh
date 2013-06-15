#!/bin/bash

totalIn=0.0
totalOut=0.0


for i in 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59
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
