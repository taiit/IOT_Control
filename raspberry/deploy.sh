#!/bin/bash

usage() {
    echo "Usage: $0 <user@host:folder>"
    echo "Example: ./deploy.sh taihv@192.168.0.11:/home/taihv/code_raspberry"
}

main() {
    IP=$1
    SCRIPT_PATH=`dirname "$0"`
    SCRIPT_PATH=`realpath $SCRIPT_PATH`
    if [[ -z $IP ]]; then
        usage $0; exit 1
    fi
    echo "This action will replace content on $IP"
    echo "Full command: rsync -avzhe ssh \"$SCRIPT_PATH/\" \"$IP\""
    read -p "Are you sure? (Y/N) " -n 1 -r
    echo    # (optional) move to a new line
    if [[ ! $REPLY =~ ^[Yy]$ ]]
    then
        echo "Exit!!"; exit 1
    fi
   
    rsync -avzhe ssh "$SCRIPT_PATH/" "$IP"
}

####
main $@
####