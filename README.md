FidiStat
========

FreeNAS monitoring for [Status Board](http://www.panic.com/statusboard/)

**Work in Progress**

TODO
----
* HDD usage
live network traffic

Ready
-----
* load avg.
* cpu temperature
* network traffic
* HDD temperature
* automated script which generates a statusboard:// link

Install
------
* put .json files somewhere you can access them from your iPad
* make a chronjob (or something similar) to execute the .sh files regulairly
* run makeLink.sh with parameters for Username, Password and Server adress
    * <code>./makeLink.sh User Pass my.Server.com</code> 
* go to statusboardLinks.html and click the links you want to add
