#!/bin/sh
   
temps1=`smartctl -a /dev/ada0 | awk '/Temperature_Celsius/{print $0}' | awk '{print $10}'`

time=`date "+%H:%M"`
new1="                    { \"title\" : \"$time\", \"value\" : $temps1},"
awk 'NR != 10' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk -v n="$new1" 'NR == 33 {print n} {print}' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
