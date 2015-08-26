FidiStat 2.0
============

FreeBSD/FreeNAS monitoring for [Status Board](http://www.panic.com/statusboard/)

Install
------
<code>sudo make install clean</code>   
You need [/ports/devel/jansson](http://www.digip.org/jansson/), [/ports/devel/libconfig](http://www.hyperrealm.com/libconfig/) and [/ports/security/libressl](http://libressl.org) to compile from Source.
They are available in /usr/ports/ if using FreeBSD. 
When /usr/ports/ are installed, make will install them for you. Otherwise install them manually.   
Standard Destination is <code>/usr/local/bin/fidistat</code> for the binary and <code>/usr/local/etc/fidistat/</code> for the config.cfg als well as the jsons.
These can be symlinked into a Directory accessible from Status Board (That means a webserver wich distributes the files).

Usage
-----
Per default, with setting "local = true", the JSON files are generated on the local machine.    

    fidistat --client start
starts the daemon.  
alternatively:

    service fidistat start
on BSD starts via rc.d.    

To provide "clean" Timestamps, the daemon will wait until the next full<code>60min modulo interval x</code> minutes and then wakes every x minutes to update the jsons with new values.    

SSL/TLS
---
If you want to have the files on another host, or generate statusboards for several machines, you can do that via TLS.
Generate a basic certificate (advanced options will be available later):

    openssl req -x509 -nodes -days 365 -newkey rsa:4096 -keyout cert.pem -out cert.pem

and provide a URL and Port in the settings.   
Run <code>fidistat server start</code> on the server side and <code>fidistat client start</code> on the client side.    
if the *local* setting clientside is set to false, the client will attempt to send the updates to the server.

**WARNING**    
I strongly advice **NOT** to run the sever on an port acessible form the internet, at least not without firewall, or only in an own jail.   
The server expects a certain JSON format, but with a setting of type CSV, a client (or attacker) could write arbitrary code to disk.   
There is very basic authentification via a string in the config, which has to be the same on client an server.   
Since TLS is enforced, this offers some protection, but this is by no means secure, at the very least a DOS attack would be possible.    
Additionally, handling of malformed JSON is currently not well defined an may lead to crashes, bufferoverflows or worse.    

TL;DR: don't use the Server part if you are not 100% certain who has acess to it.       
**WARNING**

Migrate from 1.x to 2.x
-----------------------
The config layout has somewhat changed, the "list" array is gone, you have one "settings" list object, where settings for each status is saved as before.
The name of each status is now another setting called "name" (before: <code>$name={ ...</code> now:<code>{ name=$name; ...</code>)  
Additionally there is the clientName with identifies the fidistat for each machine. This is now part of the filename of the jsons, so you would need to modify your Statusboard URLs.   
The local boolean value indicates wether to keep the data local or send it over TLS. (this hase some new settings as well).     

In general see the provided configFiles and update your old configs acordingly.

The Last release of version 1.x is v1.3.1

config.cfg
----------
See <code>configFiles/</code> for examples; <code>configFiles/config.cfg</code> is the most basic version and should work everywhere.
Each Status needs the following settings:   

* *name* name, under with (plus clientName) the .json is saved.
* *enabled* bool to enable/disable this status
* *cmmd* string with the command that delivers the data to stdin
* *display* 
    * *title* title displayed in statusboard
    * *type* type of graph
        * *line* standard graph, each line a sequence with n time/value pairs
        * *bar* bar graphs, currently not tested
        * *csv* the command should output csv data, the *title* is used as first line (for title of each column) 
    * *other* see [here](http://www.panic.com/statusboard/docs/graph_tutorial.pdf) for other options available.
* *sequencetitles* array of titles for each sequence (line and bar graphs).
* *sequencecolors* array of colors for each sequence (line and bar graphs).

options
-------
* -C,--client [start|stop|restart]: Control Client
* -S,--server [start|stop|restart]: Control Server
* -C,--client delete: Delete all jsons from this Client
* -C,--client update: Updates display and graph settings
* -n,--now: start the client once. for testing purposes
* -f,--config CFG_File: expects path to config file. if not specified, <code>/usr/local/etc/fidistat/config.cfg</code> is used.
* -v,--verbose :verbose output
