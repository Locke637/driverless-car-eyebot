#!/bin/bash

#DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
DIR=`pwd`
#echo $DIR

if [ "$#" -ne 1 ]; then
    echo "Incorrect number of parameters (expected 1)"
    echo "usage: eyesim Path/To/SimFile.sim"
else
    result=`pgrep quartz`
    #echo $result
    if [ "$result" == "" ] ; then
      echo "Starting XQuartz"
      open /Applications/Utilities/XQuartz.app
      sleep 4s
    fi
    # delete old EyeSim process
    result=`pgrep eyesim`
    echo $result
    if [ "$result" != "" ] ; then
      kill "$result"
    fi
    open /Applications/EyeSim.app --args $1 $DIR
fi
