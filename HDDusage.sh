#!/bin/bash
filename="HDDusage.csv"

echo "Size; HDD Usage" > $filename

df -H | grep ^Data/Dokumente | awk '{ gsub("T", "000000000", $3); gsub("G", "000000", $3); gsub("M", "000", $3); gsub("k", "", $3);  print "Dokumente;" , $3}' >> $filename

df -H | grep ^Data/Filme | awk '{ gsub("T", "000000000", $3); gsub("G", "000000", $3); gsub("M", "000", $3); gsub("k", "", $3);  print "Filme;" , $3}' >> $filename 

df -H | grep ^Data/Musik | awk '{ gsub("T", "000000000", $3); gsub("G", "000000", $3); gsub("M", "000", $3); gsub("k", "", $3);  print "Musik;" , $3}' >> $filename

df -H | grep ^Data/Programme | awk '{ gsub("T", "000000000", $3); gsub("G", "000000", $3); gsub("M", "000", $3); gsub("k", "", $3);  print "Programme;" , $3}' >> $filename

df -H | grep ^Data/Spiele | awk '{ gsub("T", "000000000", $3); gsub("G", "000000", $3); gsub("M", "000", $3); gsub("k", "", $3);  print "Spiele;" , $3}' >> $filename 

df -H | grep ^Data/Filme | awk '{ gsub("T", "000000000", $2); gsub("G", "000000", $2); gsub("M", "000", $2); gsub("k", "", $2);  print "Gesamt;" , $2}' >> $filename 



