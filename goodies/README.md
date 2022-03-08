# Goodies

This forder contains useful stuff.

## `italyprog.zip`

This is an enhanced keyboard map for italian keyboards that adds two shortcuts for characters that are often sed in programming and on linux: tilda `~` and backtick `.

Those characters are mapped to `AltGR` + `'` and `AltGr` + `ì`, respectively.

**NOTE: this is only needed if you have a physical italian keyboard**

To install it, proceed as follows:

1. unzip
2. run the installer `setup.exe`
3. Go to Settings > Time & language > Language & region, then click on the dots beside your language (Italian), then under the section "keyboard" click on "Add a keyboard" and search for "Italian - Programmer keys"
4. optionally, remove the original italian keymap

## MQTT broker

The MQTT broker can be launched using the Docker container `eclipse-mosquitto`. Note that it must be launched from the PowerShell console and **not** from the WSL console, for the latter runs a virtual machine with its own network that is not reachable from the outside. So, open the PowerShell in this folder and type:

```sh
docker run -it --rm -p1883:1883 -v"${PWD}/mosquitto.conf:/mosquitto/config/mosquitto.conf" eclipse-mosquitto
```

Note that that the `mosquitto.conf` file must be in the current directory.
