#!/bin/sh
if [[ $# == 3 ]]
then
    rm statboardlinks.html
    touch statboardlinks.html

    echo "<html>
    <body>
    <h1>fidistat</h1>

    <p>freenas monitoring for panic&#8217;s <a href="http://www.panic.com/statusboard/">staus board</a></p>

    <h2>Click these links on an iPad:</h2>
    " >> statboardLinks.html
    find json/*.json | while read i
    do
        echo "<a href=\"panicboard://?url=ftp%3A%2F%2F$1%3A$2%40$3/$i&panel=graph&sourceDisplayName=$(hostname)\">$i</a><br>" >> statboardLinks.html
    done

    echo "</body>
    </html>" >>statboardLinks.html
elif [[ $# == 1]]
then
    rm statboardlinks.html
    touch statboardlinks.html

    echo "<html>
    <body>
    <h1>fidistat</h1>

    <p>freenas monitoring for panic&#8217;s <a href="http://www.panic.com/statusboard/">staus board</a></p>

    <h2>Click these links on an iPad:</h2>
    " >> statboardLinks.html
    find *.json | while read i
    do
        echo "<a href=\"panicboard://$1\">$i</a><br>" >> statboardLinks.html
    done

    echo "</body>
        </html>" >>statboardLinks.html
else
    echo "You need either 3 parameters, ftp://<User>:<Password>@<Server address>/"
    echo "or 1 parameter for personal adress (\" \" escaped)"
fi
