# Using the Ubuntu PPA #
We provide a [PPA](https://launchpad.net/ubuntu/+ppas) for Ubuntu, so that you can install and update the refbox as a package.
Detailed instructions for adding the PPA to your system can be found on https://code.launchpad.net/~ssl-auto-ref/+archive/main

In short:
```
sudo add-apt-repository ppa:ssl-auto-ref/main
sudo apt-get update
sudo apt-get install ssl-autonomous-refbox
```
And to update:
```
sudo apt-get update
sudo apt-get upgrade
```

If you prefer using the GUI, use **ppa:ssl-auto-ref/main** as repository and install the **ssl-autonomous-refbox** package

The refbox can be started from menu or by executing "ssl-autonomous-refbox".

# Manual Install #
## Requirements ##

This software needs: Ubuntu, OpenGL, QT (4 or higher), SWI-Prolog, Google Protobuf, g++

On Ubuntu, you can install everything by running
```
sudo apt-get install build-essential autoconf libtool git freeglut3-dev qt4-qmake swi-prolog libqt4-dev libprotobuf-dev liblog4cxx10-dev protobuf-compiler g++ libqt4-opengl-dev
```
If there are problems with the git package, try using **git-core** instead.

## Download project ##

To get the latest stable version, checkout a stable tag
```
git clone https://code.google.com/p/ssl-autonomous-refbox/
cd ssl-autonomous-refbox/

# list available tags
git tag -l

# checkout a tag (e.g. 110315_stable)
git checkout <tagname>
```
Alternatively, go to the source tab and follow the instructions, stated there.
## Compile project ##

compile (and optionally install) project by running
```
make clean
./configure
make
sudo make install # optional
```
in the project root.


## Run the project ##

You can run the refbox by entering
```
./startrefbox.sh
```
Binary and config files are in ./bin


## Integrating into an IDE ##

If you are looking for an IDE, you might have a look at qDevelop or eclipse.
However, you should also consider using Vim :)