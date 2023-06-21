#!bin/bash
sudo apt update
sudo apt -y upgrade
sudo apt -y install ibus-mozc
ibus restart
gsettings set org.gnome.desktop.input-sources sources "[('xkb', 'jp'), ('ibus', 'mozc-jp')]"

curl -s https://raw.githubusercontent.com/karaage0703/ubuntu-setup/master/install-vscode.sh | /bin/bash
sudo apt-get -y install thonny
mkdir pico
cd pico
git clone -b master https://github.com/raspberrypi/pico-sdk.git
git clone -b master https://github.com/raspberrypi/pico-examples.git
git clone -b master https://github.com/raspberrypi/pico-playground.git
git clone -b master https://github.com/raspberrypi/pico-extras.git
git clone https://github.com/Akatoki-Saidai/SC-17.git
sudo apt install -y cmake gcc-arm-none-eabi libnewlib-arm-none-eabi build-essential

export PICO_SDK_PATH=~/pico/pico-sdk >> .profile
export PICO_EXTRAS_PATH=~/pico/pico-extras >> .profile
cd ~/
cd pico/pico-playground/
mkdir build
cd build/
cmake ..
cd apps/usb_sound_card
make

cd ~/
cd pico
export PICO_SDK_PATH=$PWD/pico-sdk
cd pico-examples
mkdir build
cd build
cmake ..
make
cd ~/
wget https://repo.anaconda.com/archive/Anaconda3-2023.03-1-Linux-x86_64.sh
bash Anaconda3-2023.03-1-Linux-x86_64.sh
echo "source ~/anaconda3/etc/profile.d/conda.sh" >> ~/.zshrc
source ~/.zshrc
sudo wget https://github.com/shiftkey/desktop/releases/download/release-3.1.1-linux1/GitHubDesktop-linux-3.1.1-linux1.deb
sudo apt -y install ~/GitHubDesktop-linux-3.1.1-linux1.deb
LANG=C xdg-user-dirs-gtk-update
