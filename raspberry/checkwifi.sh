# !/bin/bash
ping -c4 192.168.1.1 > /dev/null
 
if [ $? != 0 ] 
then
  sudo ifconfig wlan0 down
  sleep 5
  sudo ifconfig wlan0 up
fi

# sudo chmod 775 /usr/local/bin/checkwifi.sh

# Add it to crontab job

# crontab -e
# */5 * * * * /usr/bin/sudo -H /usr/local/bin/checkwifi.sh >> /dev/null 2>&1

