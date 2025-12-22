#!/bin/bash
# Script bật WiFi Hotspot

SSID="MyHotspot"
PASS="12345678"
IFACE="wlp1s0"   # đổi nếu card WiFi của bạn tên khác (xem bằng: nmcli dev status)

echo "Bật hotspot SSID=$SSID PASS=$PASS trên interface $IFACE ..."
nmcli dev wifi hotspot ifname "$IFACE" ssid "$SSID" password "$PASS"

# In ra IP của hotspot
nmcli dev show "$IFACE" | grep IP4.ADDRESS
