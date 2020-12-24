# PreSonus StudioLive Console Advertisement

> UDP discovery packets are periodically (every 3 seconds) broadcasted from StudioLive III consoles, so that remote applications such as `UC Surface` and `Universal Control` can discover them.  
In scenarios where these UDP broadcast packets do not reach the device (i.e. firewall or routing (mis)configurations), a device may be unable to know the existence of a console.

This code snippet crafts a discovery packet through raw sockets, such that the source IP can be spoofed.

---

_Currently **Linux only**, because idk how to use `winsock2` on Windows ☞(⌒▽⌒)☜_

---

Usage (as root): `./advertise <spoof_ip> [--once]`

* `spoof_ip` - The IP address to spoof - this should be the address of your console
* `--once` - _(optional)_ - Send only one packet, instead of periodically

Until I implement a ~better~ proper command line argument parser, you can modify the `modelString`, `serialNumber` and `friendlyName` strings in the source code to change the details of the advertised console

---

## Research and Development

Read more [here](https://featherbear.cc/presonus-studiolive-api/protocol.html)
