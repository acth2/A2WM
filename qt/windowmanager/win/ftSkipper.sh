#!/bin/bash

# 60Hz means 0.0167 seconds
refresh_rate=0.0167

while true; do
    start=$(date +%s.%N)  # Record the start time
    xdotool mousemove_relative --sync 0 0
    end=$(date +%s.%N)  # Record the end time
    duration=$(echo "$end - $start" | bc)
    sleep_time=$(echo "$refresh_rate - $duration" | bc)
    if (( $(echo "$sleep_time > 0" | bc -l) )); then
        sleep "$sleep_time"
    fi
done
