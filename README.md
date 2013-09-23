FidiStat
========

FreeNAS monitoring for [Status Board](http://www.panic.com/statusboard/)


TODO
----
* Makefile

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
* You need [Jansson](http://www.digip.org/jansson/) and [Libconfig](http://www.hyperrealm.com/libconfig/)
* <code>gcc -lconfig -ljansson main.c regex.c config.c jansson.c -o status</code>
    * makefile will be provided later
* make a new folder and copy the .jsons from json-base
* Modify the config.cfg file to match your needs and the specific commands, depending on the Platform


config.cfg
----------
You provide a Shell command and a Regex to parse the actual data, and the Programm will generate a .json or .csv file
you can import to Status Board.app
The Program expects to get the Data in the same order it has to write them to the File.
When you want to add or modify a setting, you have to create a new .json file, templates can be found in json-base 


