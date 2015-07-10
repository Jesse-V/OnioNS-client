#OnioNS - the Onion Name System
### Tor-Powered Distributed DNS for Tor Hidden Services

The Onion Name System (OnioNS) is a privacy-enhanced, distributed, and highly usable DNS for Tor hidden services. It allows users to reference a hidden service by a meaningful globally-unique domain name chosen by the hidden service operator. The system is powered by the Tor network and relies on a distributed database. This project aims to address the major usability issue that has been with Tor hidden services since their introduction in 2002. The official project page is onions55e7yam27n.onion, which is example.tor under OnioNS.

### Repository Details

This repository provides the client-side functionality and the integration with the Tor Browser.

### Supported Systems

Please see the [OnioNS-common README](https://github.com/Jesse-V/OnioNS-common#supported-systems) for more information.

### Installation

There are several methods to install the OnioNS software. The method of choice depends on your system. If you are on Ubuntu or an Ubuntu-based system (Lubuntu, Kubuntu, Mint) please use the PPA method. If you are running Debian or prefer not to use my PPA, please use the .deb method. Otherwise, for all other distributions, please install from source.

* **Install from PPA**

> 1. **sudo add-apt-repository ppa:jvictors/tor-dev**
> 2. **sudo apt-get update**
> 3. **sudo apt-get install tor-onions-client**

This is the recommended method as it's very easy to stay up-to-date with my releases.

* **Install from .deb file**

I provide amd64 .deb builds in the [Releases section](https://github.com/Jesse-V/OnioNS/releases), which should work for you. For other architectures, you may download from [my PPA](https://launchpad.net/~jvictors/+archive/tor-dev/+packages).

* **Install from source**

> 1. Follow [the instructions in the OnioNS-common repository](https://github.com/Jesse-V/OnioNS-common#installation) to install the tor-onions-common package.
> 2. Download the latest .zip or .tar.gz archive from the Releases page and unzip it.
> 3. Debian/Ubuntu/Mint: **sudo apt-get install python-stem** Fedora: **yum install python-stem**
> 4. **./build.sh**
> 5. **cd build/**
> 6. **sudo make install**

The ClangBuild.sh script is available if you prefer the Clang compiler. This script is recommended if you are developing or hacking OnioNS. You will need to install *clang-format-3.6* before running that as ClangBuild.sh will also re-style your code to the official development style, which is based on Chromium.

### Initialization

I strongly recommend that you follow the below procedures to integrate the software into the Tor Browser. This will make it extremely easy to access .tor domains, and I have made the software with the Tor Browser in mind. It is usually not necessary to do this process more than once.

> 1. Open a terminal and navigate to where you have extracted the Tor Browser. For example, on my machine this is *~/tor-browser_en-US* but you may have placed it somewhere else. You should see a "Tor Browser" executable and a directory named "Browser".
> 2. **mv Browser/TorBrowser/Tor/tor Browser/TorBrowser/Tor/torbin**
> 3. **ln -s /usr/bin/onions-tbb Browser/TorBrowser/Tor/tor**
> 4. **mkdir -p Browser/TorBrowser/OnioNS**

This replaces the normal Tor binary with a executable that launches the original Tor binary and then the OnioNS software as child processes, allowing the OnioNS software to start when the Tor Browser starts. This initialization is locale-independent, so all locales of the Tor Browser are supported.

### Usage

The Tor Browser operates as before, but the OnioNS software running in the background allows the Tor Browser to load hidden services under a .tor domain name.

> 1. Open the Tor Browser.
> 2. Type "example.tor" into the Tor Browser.
> 3. In a moment, you should arrive at the project's hidden service.

### Troubleshooting

If at startup the Tor Browser immediately throws a message saying "Something Went Wrong! Tor is not working in this browser." it most likely means that the OnioNS software was unable to connect to the network. This is a fatal situation, so the software aborts and the Tor Browser throws this message. Since this occurs after Tor starts, the issue is most likely not on your end. Please contact me (see below) for assistance.

If you are unable to load "example.tor", it's possible that either the hidden service is down or that the OnioNS software is not running properly on your end. To test this, visit "onions55e7yam27n.onion". If the site loads, you could try restarting the Tor Browser, which may clear the issue. If the site still does not load, please contact me for further assistance.

### Getting Help

If you have installed the software and then initialized the Tor Browser (again, a one-time operation) the software should work as intended. However, if something went wrong and you need assistance, please contact kernelcorn on #tor-dev on OFTC IRC, or email kernelcorn at riseup dot net (PGP key 0xC20BEC80). If you would like to report a bug or file an enhancement request, please [open a ticket on Github](https://github.com/Jesse-V/OnioNS-client/issues) or contact me over IRC or email if you do not have a Github account.

### How to Contribute

Most of all, I need more testers to verify that the software is stable and reliable. If you find an issue, please report it on Github. I also need inspections: of the network using Wireshark or similar tools to verify that there are no leaks, and of the code by examining the code for corner-cases or situations that I may have missed. If are a developer, I gladly accept pull requests.
