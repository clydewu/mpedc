#!/bin/bash

NET_CONF="edc_network.conf"

IP=`grep ^ip $NET_CONF | awk -F = '{print $2}'`
SUBMASK=`grep ^submask $NET_CONF | awk -F = '{print $2}'`
GATEWAY=`grep ^gateway $NET_CONF | awk -F = '{print $2}'`

#echo $IP
#echo $SUBMASK
#echo $GATEWAY

ifconfig eth0 $IP netmask $SUBMASK
route del default
route add default gw $GATEWAY
