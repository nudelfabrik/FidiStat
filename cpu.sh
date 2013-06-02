#!/bin/sh
cpu1=`sysctl -n dev.cpu.0.temperature`
cpu2=`sysctl -n dev.cpu.1.temperature`
time=`date "+%H:%M"`
new1="                    { \"title\" : \"$time\", \"value\" : $cpu1},"
new2="                    { \"title\" : \"$time\", \"value\" : $cpu2},"
awk 'NR != 9' cpu.json > .cpu.tmp && mv .cpu.tmp cpu.json
awk 'NR != 37' cpu.json > .cpu.tmp && mv .cpu.tmp cpu.json
awk -v n="$new1" 'NR == 32 {print n} {print}' cpu.json > .cpu.tmp && mv .cpu.tmp cpu.json
awk -v n="$new2" 'NR == 61 {print n} {print}' cpu.json > .cpu.tmp && mv .cpu.tmp cpu.json

