# PreSonus StudioLive Console Advertisement

> UDP discovery packets are periodically (every 3 seconds) broadcasted from StudioLive III consoles, so that remote applications such as `UC Surface` and `Universal Control` can discover them.  
In scenarios where these UDP broadcast packets do not reach the device (i.e. firewall or routing (mis)configurations), a device may be unable to know the existence of a console.

These code snippets craft a discovery packet through raw sockets, such that the source IP can be spoofed.

---

## Research and Development

Read more [here](https://featherbear.cc/presonus-studiolive-api)
