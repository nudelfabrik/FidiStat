#!/bin/sh
ut1=`uptime | awk '{ print $(NF-2) }' | sed 's/,/\./g'`
ut2=`uptime | awk '{ print $(NF-1) }' | sed 's/,/\./g'`
ut3=`uptime | awk '{ print $(NF)   }' | sed 's/,/\./g'`

time=`date "+%H:%M"`
new1="                    { \"title\" : \"$time\", \"value\" : $ut1},"
new2="                    { \"title\" : \"$time\", \"value\" : $ut2},"
new3="                    { \"title\" : \"$time\", \"value\" : $ut3},"
awk 'NR != 9' load.json > .load.tmp && mv .load.tmp load.json
awk -v n="$new1" 'NR == 32 {print n} {print}' load.json > .load.tmp && mv .load.tmp load.json
awk 'NR != 38' load.json > .load.tmp && mv .load.tmp load.json
awk -v n="$new2" 'NR == 61 {print n} {print}' load.json > .load.tmp && mv .load.tmp load.json
awk 'NR != 67' load.json > .load.tmp && mv .load.tmp load.json
awk -v n="$new3" 'NR == 90 {print n} {print}' load.json > .load.tmp && mv .load.tmp load.json
