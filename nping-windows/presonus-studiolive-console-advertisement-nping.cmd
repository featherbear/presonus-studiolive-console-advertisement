@echo off
rem *****************************************************
rem * Needs NPING (part of NMAP package) in same folder *
rem * Use ZIP from https://nmap.org/download#windows    *
rem *                                                   *
rem * Trick: dest-ip 127.255.255.255 works for local PC *
rem *        dest-ip 255.255.255.255 sends to network   *
rem *****************************************************

set src=192.168.11.109
if not "%1" == "" set src=%1

.\nping.exe --udp -g 53000 -p 47809 -S %src% --dest-ip 127.255.255.255 --delay 3s --count 0 -N --data 5543000108cf44416500000000040080481c48672360514f924e1e46915051d153747564696f4c6976652032345200415544005241303030303030303030300046616b6520436f6e736f6c6500

