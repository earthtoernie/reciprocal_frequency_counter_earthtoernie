#! /bin/bash

# only works on Ubuntu 23.04
# assumes VSCODE is installed (via snap)
# no picoprobe build, since using https://www.raspberrypi.com/documentation/microcontrollers/debug-probe.html
# no openocd build, since Ubuntu 23.04 version supports pico
echo "Must run inside of empty 'pico' folder in the users home"
echo "Must be run with 'sudo'"
echo "enter your username"

read LOCAL_USERNAME
echo nice to meet you $LOCAL_USERNAME

BASH_RC_FILE=/home/$LOCAL_USERNAME/.bashrc


echo $BASH_RC_FILE

# update bashrc
cat $BASH_RC_FILE | grep PICO_SDK_PATH
if [ $? -eq 0 ] 
then 
  echo "**skipping PICO_SDK_PATH"
  export PICO_SDK_PATH=/home/$LOCAL_USERNAME/pico/pico-sdk
else 
  echo export PICO_SDK_PATH=/home/$LOCAL_USERNAME/pico/pico-sdk >> $BASH_RC_FILE
  export PICO_SDK_PATH=/home/$LOCAL_USERNAME/pico/pico-sdk
fi

cat $BASH_RC_FILE | grep PICO_EXAMPLES_PATH
if [ $? -eq 0 ] 
then 
  echo "**skipping PICO_EXAMPLES_PATH" 
else 
  echo export PICO_EXAMPLES_PATH=/home/$LOCAL_USERNAME/pico/pico-examples >> $BASH_RC_FILE
  export PICO_EXAMPLES_PATH=/home/$LOCAL_USERNAME/pico/pico-examples
fi

cat $BASH_RC_FILE | grep PICO_EXTRAS_PATH
if [ $? -eq 0 ] 
then 
  echo "**skipping PICO_EXTRAS_PATH" 
else 
  echo export PICO_EXTRAS_PATH=/home/$LOCAL_USERNAME/pico/pico-extras >> $BASH_RC_FILE
  export PICO_EXTRAS_PATH=/home/$LOCAL_USERNAME/pico/pico-extras
fi

cat $BASH_RC_FILE | grep PICO_PLAYGROUND_PATH
if [ $? -eq 0 ] 
then 
  echo "**skipping PICO_PLAYGROUND_PATH" 
else 
  echo export PICO_PLAYGROUND_PATH=/home/$LOCAL_USERNAME/pico/pico-playground
  export PICO_PLAYGROUND_PATH=/home/$LOCAL_USERNAME/pico/pico-playground
fi

sudo apt install git -y

# clone all repos
git clone https://github.com/raspberrypi/pico-sdk.git
cd pico-sdk
git submodule update --init
cd ..
git clone https://github.com/raspberrypi/pico-examples.git
git clone https://github.com/raspberrypi/pico-extras.git
git clone https://github.com/raspberrypi/pico-playground.git
git clone https://github.com/raspberrypi/picotool.git

sudo chown -R $LOCAL_USERNAME:$LOCAL_USERNAME pico-examples
sudo chown -R $LOCAL_USERNAME:$LOCAL_USERNAME pico-extras
sudo chown -R $LOCAL_USERNAME:$LOCAL_USERNAME pico-playground
sudo chown -R $LOCAL_USERNAME:$LOCAL_USERNAME picotool
sudo chown -R $LOCAL_USERNAME:$LOCAL_USERNAME pico-sdk

# build picotool
sudo apt install cmake -y
# will error on "picotool cannot be built because libUSB is not found" without libUSB
sudo apt install libusb-1.0-0-dev -y

# this (pkg-config) is to make the message "cmake: PICO_SDK_PATH is not defined" go away.
#sudo apt install pkg-config -y
cd picotool
ls
mkdir build
cd build
ls
cmake ../
make
cd ..
cd ..

#set up tools
sudo apt install ninja-build -y
runuser - $LOCAL_USERNAME -c 'code --install-extension marus25.cortex-debug'
runuser - $LOCAL_USERNAME -c 'code --install-extension ms-vscode.cmake-tools'
runuser - $LOCAL_USERNAME -c 'code --install-extension ms-vscode.cpptools'

sudo apt install gcc-arm-none-eabi -y
sudo apt install gcc -y
sudo apt install g++ -y
sudo apt install openocd -y

sudo apt install gdb-multiarch -y

# sudo apt install binutils-mu