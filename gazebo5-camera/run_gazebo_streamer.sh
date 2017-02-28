#!/bin/bash
if [ -z "$1" ]
  then
    echo "No argument supplied"
    gst_home="/home/michele/gst-rtsp-server-1.2.3/"
else
    gst_home="$1"
fi

echo $gst_home

source $gst_home/devel.env
$gst_home/examples/gazebo-streamer &
gz_pid=$!
echo "gz_pid=$gz_pid"

trap "kill -9 $gz_pid; exit" SIGHUP SIGINT SIGTERM SIGKILL
while true; do
	: # Do nothing
done
echo "ZZZZZZZZZZZZZZZZZZZ KILLED ZZZZZZZZZZZZZZZZZZZZZZZZZZ"
