#!/bin/sh
ut1=`uptime | awk '{ print $(NF-2) }' | sed 's/,//g'`
ut2=`uptime | awk '{ print $(NF-1) }' | sed 's/,//g'`
ut3=`uptime | awk '{ print $(NF)   }' | sed 's/,//g'`

time=`date "+%H:%M"`
new1="                    { \"title\" : \"$time\", \"value\" : $ut1},"
new2="                    { \"title\" : \"$time\", \"value\" : $ut2},"
new3="                    { \"title\" : \"$time\", \"value\" : $ut3},"
awk 'NR != 13' load.json > .load.tmp && mv .load.tmp load.json
awk -v n="$new1" 'NR == 36 {print n} {print}' load.json > .load.tmp && mv .load.tmp load.json
awk 'NR != 42' load.json > .load.tmp && mv .load.tmp load.json
awk -v n="$new2" 'NR == 65 {print n} {print}' load.json > .load.tmp && mv .load.tmp load.json
awk 'NR != 71' load.json > .load.tmp && mv .load.tmp load.json
awk -v n="$new3" 'NR == 94 {print n} {print}' load.json > .load.tmp && mv .load.tmp load.json
