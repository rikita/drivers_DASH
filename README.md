# Development Environment
* Board : Raspberry Pi 4
* Kernel : 5.10.27
* Interface : I2C

## Kernel source
The following wiki page describes the detail of applying new kernel.
https://www.raspberrypi.org/documentation/linux/kernel/building.md

## Sample of dts
Add the following dts file to kernel directory.
The following sample assumes that the slave address is 0x0C, i.e. CAD0=CAD1=GND.

arch/arm/boot/dts/overlays/ak0991x-overlay.dts

## Patch kernel
Apply the patch to the kernel.

~~~
$ patch -p0 < rpi_5.10.27.patch
~~~

