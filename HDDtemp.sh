#!/bin/bash
j=0
smartopt=`echo $@ | awk '{opt="-n standby"; if(match(tolower($1),'/-w/')) opt=""; print opt; }'`
for i in $(sysctl -n kern.disks | awk '{for (i=NF; i!=0 ; i--) if (match($i, '/ada/')) print $i }') 
	do
        j=$((j + 1))      
  		temps[$j]=`smartctl -a /dev/$i | awk '/Temperature_Celsius/{print $0}' | awk '{print $10}'`
	done

time=`date "+%H:%M"`
temp1=${temps[1]}
temp2=${temps[2]}
temp3=${temps[3]}
temp4=${temps[4]}
temp5=${temps[5]}
new1="                    { \"title\" : \"$time\", \"value\" : $temp1},"
new2="                    { \"title\" : \"$time\", \"value\" : $temp2},"
new3="                    { \"title\" : \"$time\", \"value\" : $temp2},"
new4="                    { \"title\" : \"$time\", \"value\" : $temp4},"
new5="                    { \"title\" : \"$time\", \"value\" : $temp5},"
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
