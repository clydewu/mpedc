#!/bin/bash
EDC_SETUP=$1
ntpdate `grep server $1 | awk -F '=' '{print $2}'`
hwclock -w
