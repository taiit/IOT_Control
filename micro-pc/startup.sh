#! /bin/bash

# startup srcipt for micro-pc
#   Copy this to Cloud Sheel, then execute
#       $ gcloud compute instances add-metadata micro-pc \
#           --metadata-from-file startup-script=[path_to]/startup.sh
#   Or startup.sh stored on Google  Cloud Storage
#       gcloud compute instances add-metadata micro-pc \
#             --metadata startup-script-url=gs://BUCKET/startup.sh
# Rerunning a startup script
#   $ sudo google_metadata_script_runner startup
# Ubuntu log on: /var/log/syslog



log_file="/tmp/__startup.sh__${BASHPID}__.log"

setup_ssh_tune() {
    echo "setup ssh tuner starting.. ." >> $log_file
}

setup_background_services() {
    echo "setup background services starting.. ." >> $log_file
}

main() {
    #1. setup ssh tune
    setup_ssh_tune

}


# call main entry
main $@
# EOF