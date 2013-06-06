#!/bin/sh

filename="Info.html"

echo "<html>" > $filename

echo "<head>" >> $filename
echo "<title>Server Infos</title>" >> $filename
echo "</head>" >> $filename

echo "<body>" >> $filename
echo "<h2><u>Server</u></h2>" >> $filename

echo "<h3>Version:</h3>" >> $filename
vers=`uname -mrs`
echo "$vers" >> $filename

echo "<h3>Uptime:</h3>" >> $filename
tim=`uptime`
echo "${tim%%, load*}" >> $filename

echo "<h3>IP Adresse:</h3>" >> $filename 
ipad=`/sbin/ifconfig | grep -E 'inet.[0-9]' | grep -v '127.0.0.1' | grep -v '0.0.0.0' | awk '{ print $2}'`
echo "$ipad" >> $filename 

echo "</body>" >> $filename

echo "</html>" >> $filename
