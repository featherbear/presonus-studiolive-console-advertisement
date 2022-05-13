This allows bidirectional broadcast traffic (i.e. allowing the discovery request packet to be sent when you press "Rescan Network" on Universal Control). It is quite noisy though, and whilst you could limit the packets (i.e. `sport`, `dport`, `length`, ...) I don't think you really need this as the advertisement packets are emitted every 3 seconds anyway.

---

---

> Below is a mirror from: https://odi.ch/weblog/posting.php?posting=731

---

---

Relaying UDP broadcasts

```
iptables -t mangle -A INPUT -i eth0 -d 255.255.255.255 -j TEE --gateway 10.1.1.255
```

The above iptables rule copies broadcast traffic received on the eth0 network interface to another network interface (the one whose broadcast address is 10.1.1.255). Note that this is one-way only. We can't add a second rule for the other direction without creating an infinite packet loop. We need to play tricks with the TTL for that!

Incoming broadcast packets typically have a TTL of 64 or 128. TEE uses the kernel function nf_dup_ipv4() to copy the packet, which already decrements the TTL if the rule is in INPUT or PREROUTING. Note that a packet with TTL=0 will still be accepted by the destination, but will no longer be routed. But TEE itself does not check for TTL=0 and happily copies such packets. So we need to prevent that too, since what we do is effectively routing.

The improved rule adds TTL sanity check:

```
iptables -t mangle -A INPUT -i eth0 -d 255.255.255.255 -m ttl --ttl-gt 0 -j TEE --gateway 10.1.1.255
```

If we want to add a rule for the other direction as well...

```
iptables -t mangle -A INPUT -i eth1 -d 255.255.255.255 -m ttl --ttl-gt 0 -j TEE --gateway 10.1.0.255
```

then we easily create a packet loop, since the copy of a packet on eth0 will now also match the rule on eth1. To prevent that we need to ensure that the copied packet has TTL=0. We can do that by simply setting the TTL=1 of all incoming broadcasts before passing them to TEE. Then no more loops should occur. The complete rule set for merging a broadcast domain across networks is then:

```
iptables -t mangle -A INPUT -i eth0 -d 255.255.255.255 -m ttl --ttl-gt 0 -j TTL --ttl-set 1
iptables -t mangle -A INPUT -i eth1 -d 255.255.255.255 -m ttl --ttl-gt 0 -j TTL --ttl-set 1
iptables -t mangle -A INPUT -i eth0 -d 255.255.255.255 -m ttl --ttl-gt 0 -j TEE --gateway 10.1.1.255
iptables -t mangle -A INPUT -i eth1 -d 255.255.255.255 -m ttl --ttl-gt 0 -j TEE --gateway 10.1.0.255
```

Make sure to monitor your broadcast traffic to detect any misconfiguration after that change:

```
tcpdump -vnpi eth0 ip broadcast
```
