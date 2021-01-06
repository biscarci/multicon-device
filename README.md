# Multicon Device Application

This is the Multicon application designed tu works on all devices equipped with OpenWrt OS.
The Multicon application for devices allows you to remotely configure the device by receiving commands through the MQTT protocol.
Using a room (obtained using the device serial number), the application listens for any commands published by the serve.

## Prerequisities
For the Multicon application development, the Teltonika SDK packages for RUT2xx, RUT850 and RUT9xx routers. The resources used in the provided examples are:

A RUT955 router
Ubuntu 16.04 OS
SDK version RUT955_R_00.06.00.4


## Installation

For the installation, follow the steps:
1. Download from the official site of Teltonika [Software Development Kit (SDK)](https://wiki.teltonika-networks.com/view/Software_Development_Kit)
2. Unpack the .tar.gz in /home/buildbot/source
3. Update and install your 'feeds' packages to avoid future problems. You can do that using:
```
./scripts/feeds update -a
./scripts/feeds install -a
```
4. Build the target-independent tools and the cross-compilation toolchain:
```
make toolchain/install
```
5. Adjust the PATH variable
```
export PATH=/home/buildbot/source/staging_dir/host/bin:$PATH
```
6. Select the Multicon Application. RType in the terminal `make menuconfig`, choose the package in Custom Applications section
7. Copy in main folder of the SDK all files in this repository
8. Compile the package using ``./multicon_develop.sh package compile``
9. Upload new multicon package ``./multicon_upgrade.sh``


## Development Guidelines

The script ``multicon_develop.sh`` allows to do the following operation specifing ``package`` or ``develop`` as parameter
1. Compile and launch the Multicon Application on Ubuntu ``./multicon_develop.sh develop run``
2. Compile the Multicon Application on Ubuntu ``./multicon_develop.sh develop compile``
3. Clear compiled sources and the executable of the Multicon Application on Ubuntu ``./multicon_develop.sh develop clear``
4. Compile the Multicon Application for the OpenWrt package ``./multicon_develop.sh package compile``
5. Clear compiled sdk sources ``./multicon_develop.sh package clear``


## Usage guide

Multicon Application receives through mqtt protocol command in json format.
The json has the following fields:
```
{
    id: 1, 
    method: "uci", 
    token:"chiave-di-autenticazione", 
    params: [ "uci", "get", "wireless.@wifi-iface[0].ssid"] 
}
```

The method field can be
- ``uci`` for the uci commands
- ``call`` for the others shell commands

The params field must be an array of string where each element is a part of the shell command.





