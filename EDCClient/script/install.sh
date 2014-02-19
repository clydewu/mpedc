#!/bin/bash

SCRIPT_FILE="edc_net_setup.sh edc_show_ip.sh set_time.sh install.sh rc"
MAIN_BIN="EDCClient show_screen ptr_init"

chmod 775 $SCRIPT $MAIN_BIN

# install cron
if [ ! -d /mnt/disk ];then
    echo "Path '/mnt/disk' is not exist. Please make it mounted"
    exit -1
fi
mv cron /mnt/disk
chown root@root /mnt/disk/cron
chmod 4755 /mnt/disk/cron

if [ ! -d /etc/cron.d/ ]; then
    mkdir -p /etc/cron.d/
fi
mv EDCClient.cron /etc/cron.d/

# Set localtime file
mv localtime /etc

#mv /etc/rc /etc/rc.`date +%Y%m%d%H%M%S`.bak
mv /etc/rc /etc/rc.`date +%Y%m%d%H%M%S`.bak
mv ./rc /etc/rc

