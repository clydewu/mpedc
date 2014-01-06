#!/bin/bash

NET_CONF=$1
SHOWSCREEN=/home/test/show_screen

IP=`grep ^ip $NET_CONF | awk -F = '{print $2}'`
SUBMASK=`grep ^submask $NET_CONF | awk -F = '{print $2}'`
GATEWAY=`grep ^gateway $NET_CONF | awk -F = '{print $2}'`
MAC=`ifconfig |grep HWaddr | awk '{print $5}' | awk -F ':' '{print $1$2$3$4$5$6}'`

$SHOWSCREEN $IP $SUBMASK $GATEWAY $MAC
