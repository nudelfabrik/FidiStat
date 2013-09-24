FidiStat
========

FreeBSD/FreeNAS monitoring for [Status Board](http://www.panic.com/statusboard/)

Ready
-----
* load avg.
* CPU temperature
* HDD temperature
* network traffic
* Disk space used
* last Logins

Install
------
<code>sudo gmake install clean</code>  
You need [Jansson](http://www.digip.org/jansson/) and [Libconfig](http://www.hyperrealm.com/libconfig/)
They are available in /usr/ports/ if using FreeBSD. When /usr/ports/ are installed, gmake will install them for you.
Standard Destination is <code>/usr/local/bin/fidistat</code> for the binary and <code>/usr/local/etc/fidistat</code> for the config.cfg als well as the jsons.
These can be symlinked into a Directory accessible from Status Board.
At last, add fidistat to your crontab to log your data every x Minutes.


config.cfg
----------
You provide a Shell command and a Regex to parse the actual data, and the Programm will generate a .json or .csv file
you can import to Status Board.app
The Program expects to get the Data in the same order it has to write them to the File.
When you want to add or modify a setting, you have to create a new .json file, templates can be found in json-base 


