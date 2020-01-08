Goblin Camp is long abandoned project. It is based on libboost and libboost
is quite sensitive to version changes (even compiler changes are seems to be important),
so it is hard to build it in modern systems. I managed to build and run it in Debian Wheezy
chroot. It was not easy, I even had to change building system from bjam to cmake (moving to cmake
was more easy than finding out why bjam fails)

So here goes explanation how to do it yourself.

1. Creating chrootred environment 

1.1 Create chroot-container using `debootstrap`
```
  # debootstrap wheezy ./wheezy http://archive.debian.org/debian
```

1.2 Create mount scrip that will mount all special filesystems to your container

```
  #!/bin/sh

  sudo mount /tmp  ./wheezy/tmp -o bind
  sudo mount /proc ./wheezy/proc -o bind
  sudo mount /dev  ./wheezy/dev -o bind
  sudo mount /sys  ./wheezy/sys -o bind

  xhost +
```
you should run this script once after reboot. Do not run it twice. `xhost +` is about
telling the xserver to accept connections from everywhere (including your chrooted system).
This will allow you to run programs in chroot and see it's window on your host xwindows system.

1.3. Checking chroot
```
    # chroot ./wheezy
```
you should get another root prompt. Try
```
  # cat /etc/issue
```
if you get `Debian GNU/Linux 7 \n \l` you are right inside the chroot

1.4. Check if you can run graphical programs from your chroot. To do this
we will install `mesa-utils` that has `glxgears` program, that is often 
used to test the graphic system
```
    # apt-get install mesa-utils
```
and then try to run
```
    #  glxgears
```
This should show you window with three rolling gears.

2. Installing build envoirement and get the source

2.1. Inside the chroot container run
```
    # apt-get install git build-essential cmake libsdl1.2-dev libsdl-image1.2-dev libsdl-image1.2-dev libboost-system1.49.0 \
    libboost-iostreams1.49-dev libboost-filesystem1.49-dev libboost-python1.49-dev libboost-thread1.49-dev locales
```

2.2. Go to the root's home dir, clone the repo and go to that dir
```
    # cd ~;  git clone -b cmake-build  https://gitlab.com/dhyannataraj/goblin-camp.git
    # cd goblin-camp/
```

3. Building

3.1. create build dir and change to it
```
    # mkdir cmake-build ; cd cmake-build
```

3.2. Run cmake, and then make

    # cmake ..

    # make

4. Running

4.1 To run Goblin Camp you need to create an en_US.UTF8 locale. To do this run
```
    # dpkg-reconfigure locales
```
and choose 'en_US.UTF-8 UTF-8' locale in both dialog windows

4.2. For some reason Goblin Camp will look for shared files in `/root/goblin-camp/share/goblin-camp`.
Let's not spend time figuring out why, and just put all needed files there
```
    # mkdir -p ../share/goblin-camp
    # cd ../share/goblin-camp ; ln -s ../../Goblin\ Camp/lib lib; cd ../../cmake-build
    # cp ../Goblin\ Camp/terminal.png ../share/goblin-camp
```

4.3 Let's run it
```
    # LANG=en_US.UTF8 ./GoblinCamp
```

5. Enjoy playing Goblin Camp!

