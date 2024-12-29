#!/bin/sh

# Link jacktrip to app binary
mkdir -p /usr/local/bin
rm -f /usr/local/bin/jacktrip
ln -s "$2"/Contents/MacOS/qjacktrip /usr/local/bin/qjacktrip
[ ! -f "/usr/local/bin/jacktrip" ] && ln -s qjacktrip /usr/local/bin/jacktrip

# Open JackTrip on intaller finish
#open -a /Applications/JackTrip.app
exit 0
