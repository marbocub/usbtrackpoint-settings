What is this?
=============

It's a tool that can be used to set the TrackPoint sensitivity of the
"Lenovo ThinkPad USB Keyboard with TrackPoint".

Requirements and Limitations
============================

This tool works with the libusb-1.0.
We have verified that the run correctly on Linux 3.2 and MacOSX 10.6.

Usage
=====

    Usage: usbtrackpoint-settings [sensitivity]
    
### Sensitivity
The higher the sensitivity, the less force you need to move the mouse cursor. Range 1-255.

Building and Installing
=======================

Compile
-------

    cc usbtrackpoint-settings.c -lusb-1.0 -o usbtrackpoint-settings

You need libusb-1.0 headers and libraries installed.

See also
========

There's a [multi-functional tool written by Bernhard Seibold][1], it will run only on Linux.

[1]: https://github.com/bseibold/tpkbdctl

License
=======

This tool is licensed under the GNU GPL v2.

