FidiStat
========

FreeNAS monitoring for [Status Board](http://www.panic.com/statusboard/)

**Work in Progress**

TODO
----
* Implement the rest of the Stats
* Automate the systemcommands
* Makefile

Ready
-----
* load avg.
* cpu temperature
* network traffic
* HDD temperature

Install
------
* You need [Jansson](http://www.digip.org/jansson/) and [Libconfig](http://www.hyperrealm.com/libconfig/)
* <code>gcc -lconfig -ljansson main.c regex.c config.c jansson.c -o status</code>
    * makefile will be provided later
* make a cronjob with status
* point StatusBoard to the .json Files
