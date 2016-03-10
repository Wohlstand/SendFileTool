#!/bin/bash

#Remove files older than 1 day (install this script into cron!)
#Recommended time is every 10 minutes, but also you can set every 30 minutes

PATH_TO_SERVER=/var/www/sentfiles/oneday #Replace with your own path!!!

find $PATH_TO_SERVER/ -type f \! -newermt '1 day ago' -exec rm {} \; >/dev/null 2>&1
find $PATH_TO_SERVER/*/ -depth -type d -empty -delete >/dev/null 2>&1
