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
tmp=`df | grep ^/dev/ada0s2a| awk '{print $3}'`
echo "                    { \"title\" : \"Dokumente\", \"value\" : $tmp}," >> $filename


echo "
                ]
            }
        ]
    }
}" >> $filename
