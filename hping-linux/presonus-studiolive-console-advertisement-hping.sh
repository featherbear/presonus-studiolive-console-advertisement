#!/bin/sh
# ***************************************
# * Needs HPING package and root access *
# ***************************************
if [ "x$1" = "x" ] ; then
  echo "Usage $0 SRC_IP [model] [description]"
  exit 1
fi
/usr/bin/printf 'UC\x00\x01\x08\xcf\x44\x41\x65\x00\x00\x00\x00\x04\x00\x80\x48\x1c\x48\x67\x23\x60\x51\x4f\x92\x4e\x1e\x46\x91\x50\x51\xd1%s\x00AUD\x00%s\x00%s\x00'\
	"${2:-StudioLive 24R}" 'RA0000000000' "${3:-Fake Console}" > /tmp/presonus$$.pkt

sudo hping3 255.255.255.255 -i 3 --udp -a "$1" -s 53000 -k -p 47809 --file /tmp/presonus$$.pkt -d `find /tmp/presonus$$.pkt -printf "%s"`

