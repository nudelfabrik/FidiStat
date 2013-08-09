#!/bin/sh
vmtotal=`sysctl -n vm.vmtotal | awk 'NR == 3 {print}'`
ruVal=`echo $vmtotal | awk '{print $3}'`
dwVal=`echo $vmtotal | awk '{print $6}'`
pwVal=`echo $vmtotal | awk '{print $9}'`
slVal=`echo $vmtotal | awk '{print $11}' | sed 's/)//g'`


time=`date "+%H:%M"`

ru="                    { \"title\" : \"$time\", \"value\" : $ruVal},"
dw="                    { \"title\" : \"$time\", \"value\" : $dwVal},"
pw="                    { \"title\" : \"$time\", \"value\" : $slVal},"
sl="                    { \"title\" : \"$time\", \"value\" : $slVal},"
awk 'NR != 10' proc.json > .proc.tmp && mv .proc.tmp proc.json
awk -v n="$ru" 'NR == 33 {print n} {print}' proc.json > .proc.tmp && mv .proc.tmp proc.json
awk 'NR != 39' proc.json > .proc.tmp && mv .proc.tmp proc.json
awk -v n="$dw" 'NR == 62 {print n} {print}' proc.json > .proc.tmp && mv .proc.tmp proc.json
awk 'NR != 68' proc.json > .proc.tmp && mv .proc.tmp proc.json
awk -v n="$pw" 'NR == 91 {print n} {print}' proc.json > .proc.tmp && mv .proc.tmp proc.json
awk 'NR != 97' proc.json > .proc.tmp && mv .proc.tmp proc.json
awk -v n="$sl" 'NR == 120 {print n} {print}' proc.json > .proc.tmp && mv .proc.tmp proc.json
