This `iptables` rule mirrors `udp/53000` packets (from `[INTERFACE]`) destined to 255.55.255.255:47809 (UDP), forwarding them to `[DEST_IP]`

```
iptables -t mangle -A PREROUTING -i [INTERFACE] -p udp --sport 53000 --dport 47809 -d 255.255.255.255 -j TEE --gateway [DEST_IP]
```
e.g. The console is on a network accessible by the `br-lan` interface, and I want to broadcast it to another network with a broadcast address of `172.23.24.255`, so that my computer with (current) dynamic IP `172.23.24.220` can receive the packet  
(If the IP address of your destination is static, you can use that instead)

```
iptables -t mangle -A PREROUTING -i br-lan -p udp --sport 53000 --dport 47809 -d 255.255.255.255 -j TEE --gateway 172.23.24.255
```

