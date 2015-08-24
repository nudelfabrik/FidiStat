FidiStat 2.0-BETA
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

    fidistat client start
starts the daemon. Currently the interval is hardcoded at 10 minutes. 
To provide "clean" Timestamps, the daemon will wait until the next full 10 minutes (0:10, 0:20,...) and then wakes every 10 minutes to update the jsons with new values.    
Changing the interval is currently a TODO, see #38.

SSL
---
If you want to have the files on another host, or generate statusboards for several machines, you can do that via TLS.
Generate a basic certificate (advanced options will be available later):

    openssl req -x509 -nodes -days 365 -newkey rsa:4096 -keyout cert.pem -out cert.pem

and provide a URL and Port in the settings.   
Run <code>fidistat server start</code> on the server side and <code>fidistat client start</code> on the client side.    
if the *local* setting clientside is set to false, the client will attempt to send the updates to the server.

**WARNING**    
Currently there is no Client authentication implemented. Do **NOT** run the server on an open port to the internet.   
The server expects a certain JSON format, but with a setting of type CSV, an attacker could write arbitrary code to disk.   
Additionally, handling of malformed JSON is currently not well defined an may lead to crashes, bufferoverflows or worse.    
**WARNING**


config.cfg
----------
See <code>configFiles/</code> for examples; <code>configFiles/config.cfg</code> is the most basic version and should work everywhere.
Each Status needs the following settings:   

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
* client start: Start Client
* client stop: stop Client
* client restart: restart client
* server start: Start server
* server stop: stop server
* server restart: restart server
* client start -n: start the client once. for testing purposes
* --verbose, -v: echoes every value to syslog. 
* --dry, -d: Dry run, nothing is written to disk.
* --config CFG_File, -f CFG_File: expects path to config file. if not specified, <code>/usr/local/etc/fidistat/config.cfg</code> is used.
* --delete -x : Delete ALL jsons listed in "list".
* --clean  -c : Delet all jsons not enabled.
