ifconfig can0 down
#echo 250000 > /sys/devices/platform/FlexCAN.0/bitrate
#cat /sys/devices/platform/FlexCAN.0/bitrate
ip link set can0 type can bitrate 250000
ifconfig can0 up
