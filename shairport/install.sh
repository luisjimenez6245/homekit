apt update
apt upgrade
apt upgrade
apt install libasound2-dev
apt install build-essential git xmltoman autoconf automake 
apt install libtool libdaemon-dev libpopt-dev libconfig-dev 
apt install libasound2-dev avahi-daemon libavahi-client-dev libssl-dev
cd /home/pi/Git/
git clone https://github.com/mikebrady/shairport-sync.git
cd shairport-sync
autoreconf -i -f 
sudo ./configure --with-alsa --with-avahi --with-ssl=openssl --with-libdaemon --with-metadata  --with-systemd --with-configfiles
make
make install
systemctl enable shairport-sync
