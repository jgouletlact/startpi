# Startpi SSD1306 OLED I2C Raspberry Pi software
This is a repository that allows pattern and text to by displayed on an SSD1306 OLED display controlled by the Raspberry Pi's I2C interface.
## Ubuntu Host Cross-Compilation
1. Setup ssh on both the host Ubuntu machine and the Raspberry pi if not already present by installing the ssh package and running `sudo systemctl enable -- now ssh`.
2. If not present, generate ssh key by running `ssh-keygen -t rsa`.
3. Install development packages on the host by running the command `sudo apt install ubuntu-dev-tools cmake curl`.
4. Import Raspberry PI Foundation GPG keys on the host by running:\
  `curl -sL http://archive.raspbian.org/raspbian.public.key | gpg --import -`\
  `gpg --export 9165938D90FDDD2E > $HOME/raspbian-archive-keyring.gpg`.

5. On the host Ubuntu machine, create a file at `$HOME/rpi.sources` with lines:\
  `deb http://archive.raspbian.org/raspbian/ RELEASE main contrib non-free rpi`\
  `deb-src http://archive.raspbian.org/raspbian/ RELEASE main contrib non-free rpi`

6. Similarly, create a file at `$HOME/.mk-sbuild.rc` with these lines:\
  `SOURCE_CHROOTS_DIR="$HOME/chroots"`\
  `DEBOOTSTRAP_KEYRING="$HOME/raspbian-archive-keyring.gpg"`\
  `TEMPLATE_SOURCES="$HOME/rpi.sources"`\
  `SKIP_UPDATES="1"`\
  `SKIP_PROPOSED="1"`\
  `SKIP_SECURITY="1"`\
  `EATMYDATA="1"`

7. Check the architecture and system of the Raspberry Pi by running the following commands on the device:\
   `dpkg --print-architecture`\
   `cat /etc/os-release`\
   Keep note of them for the next steps. In my case those values were `armhf` and `bullseye` respectively.
   
8. Run the following command to create the root filesystem on the host:\
   `mk-sbuild --arch=armhf bullseye --debootstrap-mirror=http://archive.raspbian.org/raspbian/ --name=rpi-bullseye`
   If running `mk-sbuild` for the first time, accept the default `.sbuildrc` file, log out and log back into your system and run the command another time. If it completes successfuly follow on to the next steps.

9. Enter the newly created chroot environment by running `sudo schroot -c source:rpi-bullseye-armhf -u root -d /home bash` on the host machine.
10. Install dependencies by running `apt install build-essential wget unzip ca-certificates`
11. Obtain BCM2835 library by running `wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.73.tar.gz`
12. Extract and install the driver by running:\
    `tar zxvf bcm2835-1.73.tar.gz`\
    `cd bcm2835-1.73`\
    `./configure`\
    `make && sudo make install`

13. Obtain repo by running `wget https://github.com/jgouletlact/startpi/archive/refs/heads/main.zip`
14. Extract and compile program by running:\
    `unzip main.zip`\
    `cd startpi-main`\
    `make`
    
15. Copy the binary from `bin` folder to Raspberry Pi via SCP.
   
