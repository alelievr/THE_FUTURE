#!/bin/sh

osascript -e'
tell application "System Events"
    key code 3 using {control down, command down} -- control command f
end'
