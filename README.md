FidiStat
========

FreeNAS monitoring for [Status Board](http://www.panic.com/statusboard/)

**Work in Progress**

TODO
----
* HDD usage
* automated script which generates a statusboard:// link

Ready
-----
* load avg.
* cpu temperature
* network traffic
* HDD temperature

Install
------
* put .json files somewhere you can access them from your iPad
* make a chronjob (or something similar) to execute the .sh files regulairly
* run makeLinks.sh with parameters for Username, Password and Server adress
    * <code>./makeLinks.sh User Pass my.Server.com</code> 
* go to statusboardLinks.html and click the links you want to add
