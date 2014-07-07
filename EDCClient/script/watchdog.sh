#!/bin/bash

EDC_BIN="EDCClient"
EDC_MIN_THREAD=3

EDC_FOLDER="/home/EDC/"
EDC_BACKUP_FOLDER="/home/EDCBackup/"
EDC_UPDATE_FOLDER="/home/EDCUpdate/"
EDC_UPDATE_OK=$EDC_UPDATE_FOLDER"EDCUpdate.ok"
EDC_UPDATE_FILE_PREFIX="EDCUpdate"
EDC_UPDATE_FILE_SUFFFIX="tgz"

EDC_LIST="edc.list employee.list projects.list"
EDC_SETUP_FILE="comtest.ini edc_network.conf edc_setup.conf"
EDC_LOG="EDCClient.log"

# Check if EDCClient alive
pid_list=(`pidof $EDC_BIN`)
pid_amount=${#pid_list[@]}
if [  $pid_amount -lt $EDC_MIN_THREAD ]; then
    date
    echo "EDC is dead"

    # Make it dead completed
    kill -9 `pidof "$EDC_BIN"`

    EDC_UPDATE_FILE=`ls $EDC_UPDATE_FOLDER | grep "$EDC_UPDATE_FILE_PREFIX.*\.$EDC_UPDATE_FILE_SUFFFIX"`
    # Check if EDCUpdate file exist
    if [ -z "$EDC_UPDATE_FILE" ]; then
        echo "no update archive file"
    else
        rm "$EDC_UPDATE_OK"

        # Decompress new version
        cd "$EDC_UPDATE_FOLDER"
        tar -zxvf "$EDC_UPDATE_FILE"
        # Install new version
        mv "$EDC_FOLDER" "$EDC_BACKUP_FOLDER"
        mv "$EDC_UPDATE_FOLDER/EDC" "$EDC_FOLDER"
        cd $EDC_FOLDER
        # NOTE! You can do customerize behavior in install.sh of new version
        ./install.sh

        # Restore backup files
        cd $EDC_BACKUP_FOLDER
        mv $EDC_LIST $EDC_SETUP_FILE $EDC_LOG $EDC_FOLDER
        cd /home
        rm -rf $EDC_BACKUP_FOLDER
        rm "$EDC_UPDATE_FOLDER/$EDC_UPDATE_FILE" 
    fi

    # Restart EDCClient
    cd $EDC_FOLDER
    ./EDCClient&
fi

