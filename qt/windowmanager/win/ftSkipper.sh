#!/bin/bash
refresh_rate=$(xrandr | grep '*' | awk '{print $2}' | tr -d '+*')

if [[ -z "$refresh_rate" || "$refresh_rate" -le 0 ]]; then
    echo "Failed to detect refresh rate. Using default 60 Hz."
    refresh_rate=60
fi

delay=$(awk "BEGIN {print 1/$refresh_rate}")

echo "Detected refresh rate: ${refresh_rate} Hz"
echo "Adjusting loop to run every ${delay} seconds."

# Main loop
while true; do
    xdotool mousemove_relative --sync 0 0
    sleep "$delay"
done
