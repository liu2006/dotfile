#!/bin/bash 

[ -f ~/.xprofile ] && . ~/.xprofile

xrdb -merge ~/.Xresources
xrandr --dpi 144

exec dbus-run-session i3

