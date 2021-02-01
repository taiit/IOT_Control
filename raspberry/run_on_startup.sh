# !/bin/bash

#sudo service cron reload

# Run lora gw 
/home/taihv/work/IOT_Control/raspberry/lora/test/lora_reg >/dev/null 2>&1 &

# Send raspberry pi infomation to thingsboard

/home/taihv/work/IOT_Control/raspberry/thingsboard/pi_python.service.sh >/dev/null 2>&1 &
