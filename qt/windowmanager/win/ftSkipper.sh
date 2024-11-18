#!/bin/bash
prev_time=$(date +%s%N)
interval=1000000

while true; do
    current_time=$(date +%s%N)
    if (( current_time - prev_time >= interval )); then
        xdotool mousemove_relative --sync 0 0
        prev_time=$current_time
    fi
done
