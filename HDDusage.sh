#!/bin/bash

filename="HDDusage.json"

echo "Size; HDD Usage" >> $filename
echo "{
    \"graph\" : {
        \"title\" : \"HDD usage\",
        \"type\" : \"bar\",
        \"datasequences\" : [
            {
                \"title\": \"absolute\",
                \"datapoints\" : [
" >> HDDusage.json
tmp=`df -H | grep ^Data/Dokumente | awk '{ gsub("T", "000000000", $3); gsub("G", "000000", $3); gsub("M", "000", $3); gsub("k", "", $3); $3}'`
echo "                    { \"title\" : \"Dokumente\", \"value\" : $tmp}," >> $filename
tmp=`f -H | grep ^Data/Filme | awk '{ gsub("T", "000000000", $3); gsub("G", "000000", $3); gsub("M", "000", $3); gsub("k", "", $3); $3}' >> $filename` 
echo "                    { \"title\" : \"Filme\", \"value\" : $tmp}," >> $filename
tmp=`f -H | grep ^Data/Musik | awk '{ gsub("T", "000000000", $3); gsub("G", "000000", $3); gsub("M", "000", $3); gsub("k", "", $3); $3}' >> $filename`
echo "                    { \"title\" : \"Musik\", \"value\" : $tmp}," >> $filename

tmp=`f -H | grep ^Data/Programme | awk '{ gsub("T", "000000000", $3); gsub("G", "000000", $3); gsub("M", "000", $3); gsub("k", "", $3); $3}' >> $filename`
echo "                    { \"title\" : \"Programme\", \"value\" : $tmp}," >> $filename

tmp=`f -H | grep ^Data/Spiele | awk '{ gsub("T", "000000000", $3); gsub("G", "000000", $3); gsub("M", "000", $3); gsub("k", "", $3); $3}' >> $filename`
echo "                    { \"title\" : \"Spiele\", \"value\" : $tmp}," >> $filename

tmp=`f -H | grep ^Data/Filme | awk '{ gsub("T", "000000000", $2); gsub("G", "000000", $2); gsub("M", "000", $2); gsub("k", "", $2); $2}' >> $filename`
echo "                    { \"title\" : \"Filme\", \"value\" : $tmp}," >> $filename


echo "
                ]
            }
        ]
    }
}
" >> HDDusage.json
