#!/usr/bin/env bash

bash ./start_hotspot.sh
ip addr show

sudo ufw allow 5000/tcp
sudo ufw reload

python3 webserver.py
