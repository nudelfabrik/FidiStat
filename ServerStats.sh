#! /bin/sh
#
#  Usage: lstemp [ -w or -W]
#  -w / -W: Wake up a sleeping drive to take it's temperature
#
adastat () { echo -n `camcontrol cmd $1 -a "E5 00 00 00 00 00 00 00 00 00 00 00" -r - | awk '{print $10 " " ; }'` " " ; }

filename="../mnt/Log/stats.txt"

echo "==================================================" >> $filename
echo "==================================================" >> $filename
echo >> $filename

echo "System Status"  - `date` >> $filename
echo >> $filename

echo "Server:" >> $filename
sysctl -n kern.hostname >> $filename
../sbin/ifconfig | grep -E 'inet.[0-9]' | grep -v '127.0.0.1' | grep -v '0.0.0.0' | awk '{ print $2}' >> $filename
cat ../../../etc/version >> $filename

uptime | awk '{ print "\nSystem Load:",$(NF-2),$(NF-1),$(NF) }' >> $filename
echo >> $filename

echo "CPU:" >> $filename
sysctl -n hw.model >> $filename

echo "Temperature:" >> $filename
#Hier muss bei anderer CPU erweitert werden...
echo -n "CPU1:" >> $filename; sysctl -n dev.cpu.0.temperature >> $filename
echo -n "CPU2:" >> $filename; sysctl -n dev.cpu.1.temperature >> $filename
echo >> $filename
echo "Process:" >> $filename 
ps au >> $filename
echo >> $filename

echo "Virtual Memory:" >> $filename
sysctl -n vm.vmtotal >> $filename
echo >> $filename

# Wenn -F als Parameter verwendet wird, werden die Festplatten ausgelesen! Aber: smartcrl weckt festplatten aus Standby
if [ "$2" = "-F" ];
then

	echo "Drive Activity Status:" >> $filename
	for i in $(sysctl -n kern.disks | awk '{for (i=NF; i!=0 ; i--) if (match($i, '/ada/')) print $i }'); do    
		echo -n $i: >> $filename; adastat $i >> $filename
	done  
	echo >> $filename 
	echo >> $filename
	smartopt=`echo $@ | awk '{opt="-n standby"; if(match(tolower($1),'/-w/')) opt=""; print opt; }'`

	echo "HDD:" >> $filename 
	echo "Usage:" >> $filename 
	df -H >> $filename 
	echo >> $filename 

	echo "Temperature:" >> $filename
	for i in $(sysctl -n kern.disks | awk '{for (i=NF; i!=0 ; i--) if (match($i, '/ada/')) print $i }') 
	do
   		echo $i `smartctl -a  $smartopt /dev/$i | awk 'BEGIN { DevName="N/A - Drive in standby mode" } /Temperature_Celsius/{DevTemp=$10;} /Serial Number:/{DevSerNum=$3}; /Device Model:/{DevName=$3} END { print "("DevSerNum, DevName"):", DevTemp".0C" }'` >> $filename
	done
	echo >> $filename
fi
