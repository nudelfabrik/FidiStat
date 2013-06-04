#!/bin/sh
if [[ $# != 3 ]]
then
    echo "I need 3 parameters, User Password and Server address"
else
    rm statboardLinks.html
    touch statboardLinks.html

    echo "<html>
    <body>
    <h1>FidiStat</h1>

    <p>FreeNAS monitoring for Panic&#8217;s <a href="http://www.panic.com/statusboard/">staus board</a></p>

    <h2>Click these links on an iPad:</h2>
    " >> statboardLinks.html
    find *.json | while read i
    do
        echo "<a href=\"panicboard://?url=ftp%3A%2F%2F$1%3A$2%40$3/$i&panel=graph&sourceDisplayName=$(hostname)\">$i</a><br>" >> statboardLinks.html
    done

    echo "</body>
    </html>" >>statboardLinks.html
fi
