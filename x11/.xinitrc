#!/bin/bash 

[ -f ~/.xprofile ] && . ~/.xprofile

xrdb -merge ~/.Xresources
xrandr --output DisplayPort-0 --mode 2560x1440 --rate 240 --dpi 144
fastfetch &

exec dbus-run-session i3

