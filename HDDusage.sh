#!/bin/sh
filename="HDDusage.json"
rm $filename
touch $filename
#echo "Size; HDD Usage" >> $filename
echo "{
    \"graph\" : {
        \"title\" : \"Usage\",
        \"type\" : \"bar\",
        \"datasequences\" : [
            {
                \"title\" : \"HDD\",
                \"datapoints\" : [
" >> $filename 
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

tmp=`df | grep ^Data/Filme | awk '{print $2}'`
echo "                    { \"title\" : \"Gesamt\", \"value\" : $tmp}," >> $filename

echo "
                ]
            }
        ]
    }
}" >> $filename
