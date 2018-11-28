ODIN Blockchain Core
=====================

Setup
---------------------
[ODIN Core](https://odinblockchain.org/) is the original ODIN client and it builds the backbone of the network. However, it downloads and stores the entire history of ODIN transactions; depending on the speed of your computer and network connection, the synchronization process can take anywhere from a few hours to a day or more. Thankfully you only have to do this once.

Running
---------------------
The following are some helpful notes on how to run ODIN on your native platform.

### Unix

Unpack the files into a directory and run:

- `bin/32/odin-qt` (GUI, 32-bit) or `bin/32/odind` (headless, 32-bit)
- `bin/64/odin-qt` (GUI, 64-bit) or `bin/64/odind` (headless, 64-bit)

### Windows

Unpack the files into a directory, and then run `odin-qt.exe`.

### OSX

Drag ODIN-Qt to your applications folder, and then run `ODIN-Qt`.

### Need Help?

* See the documentation at the [ODIN Wiki](https://github.com/odinblockchain/wiki/wiki)
for help and more information.
* Ask for help on [BitcoinTalk](https://bitcointalk.org/index.php?topic=4798428).
* Report an issue on [Github](https://github.com/odinblockchain/odin/issues).
* Join our Discord Community [ODIN Official Discord](https://discord.me/odinblockchain).
* Join our Reddit Community [ODIN Official Reddit](https://www.reddit.com/r/OdinBlockchain/).
* Join our Telegram Community [ODIN Official Telegram](https://t.me/ODINchain).

Building
---------------------
The following are developer notes on how to build ODIN on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

- [OSX Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Gitian Building Guide](gitian-building.md)

Development
---------------------
The ODIN repo's [root README](https://github.com/odinblockahin/Odin/blob/master/README.md) contains relevant information on the development process and automated testing.

- [Developer Notes](developer-notes.md)
- [Multiwallet Qt Development](multiwallet-qt.md)
- [Release Notes](release-notes.md)
- [Release Process](release-process.md)
- [Source Code Documentation (External Link)](https://dev.visucore.com/bitcoin/doxygen/) ***TODO***
- [Translation Process](translation_process.md)
- [Unit Tests](unit-tests.md)
- [Unauthenticated REST Interface](REST-interface.md)
- [Dnsseed Policy](dnsseed-policy.md)

### Resources

* Report an issue on [Github](https://github.com/odinblockchain/odin/issues).
* Join our Discord Community [ODIN Official Discord](https://discord.me/odinblockchain).
* Join our Reddit Community [ODIN Official Reddit](https://www.reddit.com/r/OdinBlockchain/).

### Miscellaneous
- [Assets Attribution](assets-attribution.md)
- [Files](files.md)
- [Tor Support](tor.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)

License
---------------------
Distributed under the [MIT/X11 software license](http://www.opensource.org/licenses/mit-license.php).
This product includes software developed by the OpenSSL Project for use in the [OpenSSL Toolkit](https://www.openssl.org/). This product includes
cryptographic software written by Eric Young ([eay@cryptsoft.com](mailto:eay@cryptsoft.com)), and UPnP software written by Thomas Bernard.
