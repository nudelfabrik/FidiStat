#!/bin/bash
filename="HDDusage.json"
rm $filename
touch $filename
#echo "Size; HDD Usage" >> $filename
echo "{
    \"graph\" : {
        \"title\" : \"HDD usage\",
        \"type\" : \"bar\",
        \"datasequences\" : [
            {
                \"title\": \"absolute\",
                \"datapoints\" : [
" >> HDDusage.json
tmp=`df | grep ^Data/Dokumente | awk '{print $3}'`
echo "                    { \"title\" : \"Dokumente\", \"value\" : $tmp}," >> $filename

tmp=`df | grep ^Data/Filme | awk '{print $3}'` 
echo "                    { \"title\" : \"Filme\", \"value\" : $tmp}," >> $filename

tmp=`df | grep ^Data/Audio | awk '{print $3}'`
echo "                    { \"title\" : \"Audio\", \"value\" : $tmp}," >> $filename

tmp=`df | grep ^Data/Programme | awk '{print $3}'`
echo "                    { \"title\" : \"Programme\", \"value\" : $tmp}," >> $filename

tmp=`df | grep ^Data/Spiele | awk '{print $3}'`
echo "                    { \"title\" : \"Spiele\", \"value\" : $tmp}," >> $filename

tmp=`df | grep ^Data/Filme | awk '{print $3}'`
echo "                    { \"title\" : \"Gesamt\", \"value\" : $tmp}," >> $filename


echo "
                ]
            }
        ]
    }
}
" >> HDDusage.json
