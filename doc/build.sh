# install all tools needed
sudo apt-get install g++ cmake make autoconf gcc git-core mercurial wget autoconf libtool libspeex-dev libosip2-dev libexosip2-dev intltool libavcodec-dev libmediastreamer0-dev libswscale-dev libsdl-dev
sudo apt-get build-dep liblinphone2

# download firebreath source
hg clone https://firebreath.googlecode.com/hg/ firebreath
mkdir -p firebreath/projects
cd firebreath/projects

# we want debug version
export CFLAGS='-ggdb3 -fPIC'

# remove old and buggy version of linphone from your system
sudo apt-get remove liblinphone3 linphone

# download from git to firebreath projects folder
git clone git://github.com/juzna/linphone-js.git
cd linphone-js

# download submodules
git submodule init
git submodule update

# build linphone
cd 3rdParty/linphone
git submodule init
git submodule update
./autogen.sh
./configure --enable-gtk_ui=no
make && sudo make install
cd ../..

# prepare firebreath build
cd ..
./prepmake.sh

# build plugin
cd build/
make linphone

# copy generated plugin to your firefox plugin folder
mkdir -p ~/.mozilla/plugins/
cp -v bin/linphone/nplinphone.so ~/.mozilla/plugins/
