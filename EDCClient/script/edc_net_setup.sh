#!/bin/bash

NET_CONF=$1

if [ ! -f $1 ]; then
	exit -1
fi

IP=`grep ^ip $NET_CONF | awk -F = '{print $2}'`
SUBMASK=`grep ^submask $NET_CONF | awk -F = '{print $2}'`
GATEWAY=`grep ^gateway $NET_CONF | awk -F = '{print $2}'`

#echo $IP
#echo $SUBMASK
#echo $GATEWAY

/sbin/ifconfig eth0 $IP netmask $SUBMASK
/sbin/route del default
/sbin/route add default gw $GATEWAY

exit 0
