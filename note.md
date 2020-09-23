sudo vim /etc/thingsboard/conf/thingsboard.conf

sudo vim /etc/thingsboard/conf/thingsboard.yml
port: "${HTTP_BIND_PORT:80}"

# loadDemo option will load demo data: users, devices, assets, rules, widgets.
sudo /usr/share/thingsboard/bin/install/install.sh --loadDemo

# Execute the following command to start ThingsBoard:
sudo service thingsboard start

# Once started, you will be able to open Web UI using the following link:
http://localhost:80/
