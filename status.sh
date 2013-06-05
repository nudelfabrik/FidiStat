#!/bin/sh

if [ "$1" = "-F" ];
then
    cd $2
    sh HDDtemp.sh
	sh HDDusage.sh
else
    cd $1
    sh cpu.sh
    sh load.sh
    sh netTraf.sh
	sh Info.sh
    sh proc.sh
fi

