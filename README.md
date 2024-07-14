# JACKInsert LV2 Plugin

`JACKInsert` is an LV2 plugin designed to integrate a JACK application into an LV2 host. 
This allows the routing of audio signals through a JACK application that isn't available as an LV2 plugin, for example SooperLooper
The plugin is particularly suited for use with the MOD system developed by MOD Audio.
It currently is WIP.

## Features

- Routes audio from LV2 host to a JACK application.
- Supports stereo input and output.
- Currently connects to Sooperlooper  automatically.

## Installation

### Building from Source

1. **Clone the repository:**

    ```bash
    git clone https://github.com/he-leon/jackinsert.git
    cd jackinsert
    ```

2. **Build the plugin:**

    ```bash
    make
    ```

3. **Install the plugin:**

    ```bash
    make install
    ```

   This will install the plugin into the LV2 directory (`/usr/local/lib/lv2/jackinsert.lv2`). 
   Use $PREFIX and $LIBDIR to install to another path (see Makefile)

## Usage

Once installed, the `JACKInsert` plugin can be loaded in any LV2 host. It will automatically connect to SooperLoopers common stereo in- and outputs. 

In the MOD environment, use the web interface to configure and route the plugin's audio ports to other plugins or hardware inputs/outputs.

## License

This project is licensed under LGPL v3.

## Acknowledgements

Special thanks to the LV2, JACK, and MOD development communities for their tools and libraries.

