#!/bin/sh

# Wait a moment for DWM to initialize
sleep 1

# Start applications
feh --bg-fill ~/Pictures/wp8.jpg &

# Start status bar
slstatus &
