FidiStat
========

FreeNAS monitoring for [Status Board](http://www.panic.com/statusboard/)

**Work in Progress**

TODO
----
Better scripts

Ready
-----
* load avg.
* cpu temperature
* network traffic
* HDD temperature
* automated script which generates a statusboard:// link
* HDD usage
* live network traffic

Install
------
* put .json files somewhere you can access them from your iPad
* modify the .sh to your needs (number/name of partitions for HDDusage etc.)
* make a chronjob (or something similar) to execute the .sh files regulairly
* run makeLink.sh with parameters for Username, Password and Server adress
    * <code>./makeLink.sh User Pass my.Server.com</code> 
* go to statusboardLinks.html and click the links you want to add
* when .json stay in git, ignore those changes with
    * <code>git update-index --assume-unchanged *.json info.html</code>
