#OnioNS - the Onion Name System
### Tor-Powered Distributed DNS for Tor Hidden Services

The Onion Name System (OnioNS) is a privacy-enhanced, distributed, and highly usable DNS for Tor hidden services. It allows users to reference a hidden service by a meaningful globally-unique domain name chosen by the hidden service operator. The system is powered by the Tor network and relies on a distributed database. This project aims to address the major usability issue that has been with Tor hidden services since their introduction in 2002. The official project page is onions55e7yam27n.onion, which is example.tor under OnioNS.

### Repository Details

This repository provides the client-side functionality and the integration with the Tor Browser.

### Supported Systems

#### Linux

**Ubuntu 14.04+, Debian 8+, or Linux Mint 17+**

i386, amd64, and armhf architectures are supported, so it should run on most 32-bit, 64-bit, and ARM machines. If you have an ARM board (Pi, BBB, Odroid, etc) feel free to test it. A graphical interface is currently required, as you must also run the Tor Browser.

#### Windows

Not currently supported at the moment, but currently planned.

#### OS-X and *BSD

Not currently supported, support planned in the far future. I am willing to provide upstream support to anyone who wishes to port the software over there. I have not attempted to compile this code on any BSD system. My current primary focus is developing the Linux edition.

### Installation

There are several methods to install the OnioNS software. The method of choice depends on your system. If you are on Ubuntu or an Ubuntu-based system (Lubuntu, Kubuntu, Mint) please use the PPA method. If you are running Debian or prefer not to use my PPA, please use the .deb method. Otherwise, for all other distributions, please install from source.

* **Install from PPA**

> 1. **sudo add-apt-repository ppa:jvictors/tor-dev**
> 2. **sudo apt-get update**
> 3. **sudo apt-get install tor-onions-client**

This is the recommended method as it's very easy to stay up-to-date with my releases.

* **Install from .deb file**

Please see the [Releases section](https://github.com/Jesse-V/OnioNS/releases) at the top of this Github repo. I provide amd64 .deb builds there, which should work for you. Otherwise, you may also download them from [my PPA](https://launchpad.net/~jvictors/+archive/tor-dev/+packages).

* **Install from source**

> 1. Download the latest .zip or .tar.gz archive from the Releases page and unzip it.
> 2. **sudo apt-get install python-stem g++ cmake make**
> 3. **./build.sh**
> 4. **cd build/**
> 5. **sudo make install**

The ClangBuild.sh script is available if you prefer the Clang compiler. This script is recommended if you are developing or hacking OnioNS. You will need to run **sudo apt-get install clang-format-3.6** before running the script as the script will also re-style your code to the official development style, which is based on Chromium.

### Initialization

I strongly recommend that you follow the below procedures to integrate the software into the Tor Browser. This will make it extremely easy to access .tor domains, and I have made the software with the Tor Browser in mind. It is usually not necessary to do this process more than once.

> 1. Open a terminal and navigate to where you have extracted the Tor Browser. For example, on my machine this is */home/jesse/tor-browser_en-US*. You should see a "Tor Browser" executable and a directory named "Browser".
> 2. **mv Browser/TorBrowser/Tor/tor Browser/TorBrowser/Tor/torbin**
> 3. **ln -s /usr/bin/onions-tbb Browser/TorBrowser/Tor/tor**

This allows the Tor Browser to start the OnioNS software in addition to Tor.

### Usage

The Tor Browser operates as before, but the OnioNS software running in the background allows the Tor Browser to load hidden services under a .tor domain name.

> 1. Open the Tor Browser.
> 2. Type "example.tor" into the Tor Browser.
> 3. In a moment, you should arrive at the project's hidden service.

### Getting Help

A manpage is available for your convenience. You can also type **onions-client --help** for a list of flags and usage examples. Contact me on IRC or by email (see below) if you need further assistance.

### Bug Reporting

Please open a ticket on Github. If you do not have a Github account, please contact kernelcorn on #tor-dev on OFTC IRC, or email kernelcorn at riseup dot net. Please follow the same process for filing enhancement requests. I use PGP key 0xC20BEC80. I accept pull requests if you want to contribute.
