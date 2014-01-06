#!/bin/bash

SCRIPT="edc_net_setup.sh edc_show_ip.sh set_time.sh"
MAIN_BIN="test.out show_screen cron"

chmod 775 $SCRIPT $MAIN_BIN

# install cron
if [ ! -d /mnt/disk ];then
    echo "Path '/mnt/disk' is not exist."
    exit -1
fi
mv cron /mnt/disk

if [ ! -d /etc/cron.d/ ]; then
    mkdir -p /etc/cron.d/
fi
mv crontab /etc/cron.d/


# update rc
if [ `md5sum ./rc` != `md5sum /etc/rc ` ]; then
    mv /etc/rc /etc/rc.`date +%Y%m%d%H%M%S`.bak
    mv ./rc /etc/rc
fi

