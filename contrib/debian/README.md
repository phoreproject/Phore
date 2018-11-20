
Debian
====================
This directory contains files used to package curiumd/curium-qt
for Debian-based Linux systems. If you compile curiumd/curium-qt yourself, there are some useful files here.

## curium: URI support ##


curium-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install curium-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your curiumqt binary to `/usr/bin`
and the `../../share/pixmaps/curium128.png` to `/usr/share/pixmaps`

curium-qt.protocol (KDE)

