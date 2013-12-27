#!/bin/bash

TMP_DIR="EDCClient"
MAIN_BIN="test.out show_screen cron"
RESOURCE="stdfont.15f crontab rc"
LIST="edc.list employee.list projects.list"
CONF="comtest.ini edc_network.conf edc_setup.conf"
SCRIPT="edc_net_setup.sh edc_show_ip.sh set_time.sh install.sh"

if [ ! -d $TMP_DIR ]; then
    mkdir $TMP_DIR
fi

chmod 775 $MAIN_BIN $SCRIPT
cp $MAIN_BIN $RESOURCE $LIST $CONF $SCRIPT $TMP_DIR
tar zcvf $TMP_DIR".tgz" $TMP_DIR
rm -rf $TMP_DIR
