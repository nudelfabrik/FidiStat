#!/bin/bash
   
temps1=`smartctl -a /dev/ada0 | awk '/Temperature_Celsius/{print $0}' | awk '{print $10}'`
temps2=`smartctl -a /dev/ada1 | awk '/Temperature_Celsius/{print $0}' | awk '{print $10}'`
temps3=`smartctl -a /dev/ada2 | awk '/Temperature_Celsius/{print $0}' | awk '{print $10}'`
temps4=`smartctl -a /dev/ada3 | awk '/Temperature_Celsius/{print $0}' | awk '{print $10}'`
temps5=`smartctl -a /dev/ada4 | awk '/Temperature_Celsius/{print $0}' | awk '{print $10}'`

time=`date "+%H:%M"`
new1="                    { \"title\" : \"$time\", \"value\" : $temps1},"
new2="                    { \"title\" : \"$time\", \"value\" : $temps2},"
new3="                    { \"title\" : \"$time\", \"value\" : $temps3},"
new4="                    { \"title\" : \"$time\", \"value\" : $temps4},"
new5="                    { \"title\" : \"$time\", \"value\" : $temps5},"
awk 'NR != 9' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk -v n="$new1" 'NR == 32 {print n} {print}' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk 'NR != 38' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk -v n="$new2" 'NR == 61 {print n} {print}' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk 'NR != 67' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk -v n="$new3" 'NR == 90 {print n} {print}' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk 'NR != 96' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk -v n="$new4" 'NR == 119 {print n} {print}' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk 'NR != 125' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk -v n="$new5" 'NR == 148 {print n} {print}' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
