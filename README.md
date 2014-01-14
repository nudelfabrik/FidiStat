FidiStat
========

FreeBSD/FreeNAS monitoring for [Status Board](http://www.panic.com/statusboard/)

Install
------
<code>sudo gmake install clean</code>
You need [/ports/devel/jansson](http://www.digip.org/jansson/) and [/usr/devel/libconfig](http://www.hyperrealm.com/libconfig/) to compile from Source.
They are available in /usr/ports/ if using FreeBSD.
When /usr/ports/ are installed, gmake will install them for you. Otherwise install them manually with the links Provided.
Standard Destination is <code>/usr/local/bin/fidistat</code> for the binary and <code>/usr/local/etc/fidistat/</code> for the config.cfg als well as the jsons.
These can be symlinked into a Directory accessible from Status Board (That means a webserver wich distributes the files).
At last, add fidistat to your crontab to log your data every x Minutes.


config.cfg
----------
You provide a Shell command and a Regex to parse the output of the command data, and the Programm will generate a .json or .csv file 
you can import to Status Board.app
Examples can be found in FreeBSD.cfg and FreeNAS.cfg
They may have additional dependencies but you can use other commands to get data.

