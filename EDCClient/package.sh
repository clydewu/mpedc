#!/bin/bash

TMP_DIR="EDC"
MAIN_BIN="EDCClient show_screen ptr_init"

CRON_DIR="cron/"
CRON_FILE="cron EDCClient.cron"
RESOURCE_DIR="resource/"
RESOURCE_FILE="stdfont.15f localtime"
LIST_DIR="list/"
LIST_FILE="edc.list employee.list projects.list"
CONF_DIR="conf/"
CONF_FILE="comtest.ini edc_network.conf edc_setup.conf"
SCRIPT_DIR="script/"
SCRIPT_FILE="edc_net_setup.sh edc_show_ip.sh set_time.sh install.sh rc"

# !NOTE All files are placed in the same folder

# Create directory
if [ ! -d $TMP_DIR ]; then
    mkdir $TMP_DIR
fi

# Binary file
for f in ${MAIN_BIN[*]}
do
    cp $f $TMP_DIR
done

# CRON system
for f in ${CRON_FILE[*]}
do
    cp $CRON_DIR$f $TMP_DIR
done

# Necessary resource file
for f in ${RESOURCE_FILE[*]}
do
    cp $RESOURCE_DIR$f $TMP_DIR
done

# List file
for f in ${LIST_FILE[*]}
do
    cp $LIST_DIR$f $TMP_DIR
done

# Configure file
for f in ${CONF_FILE[*]}
do
    cp $CONF_DIR$f $TMP_DIR
done

# Script file
for f in ${SCRIPT_FILE[*]}
do
    cp $SCRIPT_DIR$f $TMP_DIR
done

cd $TMP_DIR
chmod 775 $MAIN_BIN $SCRIPT_FILE
cd ..
tar zcvf $TMP_DIR".tgz" $TMP_DIR
#rm -rf $TMP_DIR
