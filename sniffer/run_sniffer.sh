#!/usr/bin/env bash

BSL='ibcn-f5x-tos-bsl'
BSL_OPTS='-5 -R --invert-reset --swap-reset-test -r -e -I'

HEX='sniffer.hex'

PORT="/dev/ttyUSB0"
CHANNEL="15"

if [ -n "$1" ]
	then
		PORT=$1
fi

if [ -n "$2" ]
	then
		CHANNEL=$2
fi

echo "Programming sniffer"
$BSL $BSL_OPTS -c $PORT -p $HEX

echo "Running sniffer"
./snifferproxy dev=$PORT channel=$CHANNEL


