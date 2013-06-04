#!/bin/sh

if [ "$1" = "-F" ];
then
    cd $2
    sh HDDtemp.sh
else
    cd $1
    sh cpu.sh
    sh load.sh
    sh netTraf.sh
fi

