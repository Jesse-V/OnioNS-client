#OnioNS - the Onion Name System
### A New Privacy-Enhanced DNS for Tor Hidden/Onion Services

OnioNS is a privacy-enhanced, metadata-free, and highly-usable DNS for Tor onion services. Administrators can use the Onion Name System to anonymously register a meaningful and globally-unique domain name for their site. Users can then load the site just by typing "example.tor" into the Tor Browser. OnioNS is backwards-compatible with traditional .onion addresses, does not require any modifications to the Tor binary or network, and there are no central authorities in charge of the domain names. This project was specifically engineered to solve the usability problem with onion services.

This is the software implementation of the system described in "The Onion Name System: Tor-Powered Decentralized DNS for Tor Onion Services", which will appear in the 2017.1 issue of the Proceedings on Privacy Enhancing Technologies (PoPETS). It will also be demoed in July 2017 at the 17th PETS Symposium in Minneapolis.

### Repository Details [![Build Status](https://travis-ci.org/Jesse-V/OnioNS-client.svg)](https://travis-ci.org/Jesse-V/OnioNS-client)

This repository provides the client-side functionality and the integration with the Tor Browser.

### Supported Platforms

#### Linux

**Debian, *buntu, Mint, and Fedora**

i386, amd64, and armhf architectures are supported. The software is lightweight and should run just fine on the Raspberry Pi or the BeagleBone Black.

#### Windows, OS-X, and BSD

Not currently supported. However, I will happily welcome ports of the software to OS-X and BSD and will provide upstream support. I will also welcome a Windows port of OnioNS-client and OnioNS-common. Talk to me if you are interested in helping out; we can discuss compensation.

### Installation

* **Ubuntu/Mint? Install using Personal Package Archive (PPA)**

> 1. **sudo add-apt-repository ppa:jvictors/tor-dev**
> 2. **sudo apt-get update**
> 3. **sudo apt-get install tor-onions-client**

This is the recommended method as it will keep you up-to-date with my releases.

* **Debian? Install from .deb file**

You can find Debian packages in [Releases section](https://github.com/Jesse-V/OnioNS-client/releases). For other architectures, see [my PPA](https://launchpad.net/~jvictors/+archive/tor-dev/+packages).

* **Otherwise, install from source**

> 1. Install tor-onions-common by following [these instructions](https://github.com/Jesse-V/OnioNS-common#installation).
> 2. Download and extract the latest release from the [Releases page](https://github.com/Jesse-V/OnioNS-client/releases).
> 3. **(mkdir build; cd build; cmake ../src; make; sudo make install)**

You can cleanup your build with **rm -rf build**

### Initialization

I strongly recommend that you follow the below procedures to integrate the software into the Tor Browser. This will make it extremely easy to access .tor domains, and I have made the software with the Tor Browser in mind. It is usually not necessary to do this process more than once, but you will have to do it again if the Tor Browser updates itself.

> 1. Open a terminal and navigate to where you have extracted the Tor Browser. For example, on my machine this is *~/tor-browser_en-US* but you may have placed it somewhere else. You should see a "Tor Browser" executable and a directory named "Browser". Then open a command-line terminal at this location and run these commands:
> 2. **mv Browser/TorBrowser/Tor/tor Browser/TorBrowser/Tor/torbin**
> 3. **ln -s /usr/bin/onions-tbb Browser/TorBrowser/Tor/tor**
> 4. **mkdir -p Browser/TorBrowser/OnioNS**

This will start the OnioNS software with the Tor Browser and generate a working directory. At startup, the Tor Browser will launch the onions-tbb binary, which will then launch the normal Tor binary as a child process. This initialization is locale-independent, so all locales of the Tor Browser are supported.

### Usage

The Tor Browser operates as before, but the OnioNS software running in the background allows the Tor Browser to load onion services under a .tor domain name.

> 1. Open the Tor Browser. OnioNS is compatible with any method of connecting to the Tor network.
> 2. Wait about five seconds for the background software to be ready.
> 3. Visit "example.tor" or any other .tor name that you know to be registered.
> 4. In a moment, the Tor Browser should load a onion service.

OnioNS simply adds compatibility for .tor domains and lets all other domains go through, so you can browse the web and visit traditional onionsites as normal.

### Troubleshooting

If at startup the Tor Browser immediately throws a message saying "Something Went Wrong! Tor is not working in this browser." or if you get a message saying that Tor exited unexpectedly, it most likely means that the OnioNS software was unable to connect to its network. This is a fatal situation, so the software aborts and the Tor Browser throws this message. Please contact me (see below) for assistance.

If you are unable to load "example.tor" more than 20 seconds after starting the Tor Browser, it's possible that either the onion service is down or that the OnioNS software is not running properly on your end. Try to visit "onions55e7yam27n.onion". If the site loads, you could try closing and reopening the Tor Browser, which may clear the issue. If the site still does not load, please contact me for further assistance.

### Getting Help

If you have installed the software and then initialized the Tor Browser (again, a one-time operation) the software should work as intended. However, if something went wrong and you need assistance, please contact kernelcorn on #tor-dev on OFTC IRC, or email kernelcorn at torproject dot org (PGP key 0xC20BEC80). If you would like to report a bug or file an enhancement request, please [open a ticket on Github](https://github.com/Jesse-V/OnioNS-client/issues) or contact me over IRC or email if you do not have a Github account.

### Contributing

Please file a Github issue ticket to report a bug or request a feature. Developers should use Clang 3.8 as it will compile faster and provide cleaner error messages. Feel check out the devBuild.sh and scanBuild.sh scripts as they can helpful to you. If you would like to contribute code, please fork this repo, sign your commits, and file a pull request.

I develop using Clang 3.8 on Debian Testing amd64.

### How to Contribute

I need more testers to verify that the software is stable and reliable. If you find an issue, please report it on Github. Developers should use Clang 3.8 as it will compile faster and provide cleaner error messages. Feel check out the devBuild.sh and scanBuild.sh scripts as they can helpful to you. If you would like to contribute code, please fork this repo, sign your commits, and file a pull request.

I develop using Clang 3.8 on Debian Testing amd64.
