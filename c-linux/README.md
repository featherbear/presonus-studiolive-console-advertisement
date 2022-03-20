Usage (as root): `./advertise <spoof_ip> [--once]`

* `spoof_ip` - The IP address to spoof - this should be the address of your console
* `--once` - _(optional)_ - Send only one packet, instead of periodically

Until I implement a ~better~ proper command line argument parser, you can modify the `modelString`, `serialNumber` and `friendlyName` strings in the source code to change the details of the advertised console
