#!/bin/sh
   
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
awk 'NR != 10' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk -v n="$new1" 'NR == 33 {print n} {print}' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk 'NR != 39' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk -v n="$new2" 'NR == 62 {print n} {print}' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk 'NR != 68' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk -v n="$new3" 'NR == 91 {print n} {print}' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk 'NR != 97' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk -v n="$new4" 'NR == 120 {print n} {print}' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk 'NR != 126' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
awk -v n="$new5" 'NR == 149 {print n} {print}' HDDtemp.json > .HDDtemp.tmp && mv .HDDtemp.tmp HDDtemp.json
