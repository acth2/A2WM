#!/bin/bash
while true; do
    idle_time=$(xprintidle)
    if [ "$idle_time" -gt 500 ]; then  # Run only if idle > 500ms
        xdotool mousemove_relative --sync 0 0
    fi
done
