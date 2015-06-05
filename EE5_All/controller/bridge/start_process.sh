#!/bin/bash
number_of_args=$#
if [ $# -lt 1 ] ; then
   echo -e "Hey! I need at least 1 command line args\n"
   echo -e "Syntax: $0: database-name.db\n"
   echo -e "Example:$0 mqtt.db"
   exit 1
fi
echo "the given database name is: $1"
./zigbee_network $1
