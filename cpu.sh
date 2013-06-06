#!/bin/sh
cpu1=`sysctl -n dev.cpu.0.temperature | sed 's/C//g'`
cpu2=`sysctl -n dev.cpu.1.temperature | sed 's/C//g'`
time=`date "+%H:%M"`
new1="                    { \"title\" : \"$time\", \"value\" : $cpu1},"
new2="                    { \"title\" : \"$time\", \"value\" : $cpu2},"
awk 'NR != 10' cpu.json > .cpu.tmp && mv .cpu.tmp cpu.json
awk -v n="$new1" 'NR == 33 {print n} {print}' cpu.json > .cpu.tmp && mv .cpu.tmp cpu.json
awk 'NR != 39' cpu.json > .cpu.tmp && mv .cpu.tmp cpu.json
awk -v n="$new2" 'NR == 62 {print n} {print}' cpu.json > .cpu.tmp && mv .cpu.tmp cpu.json

