# THE_FUTURE

Network program to render synchronously shaders on multiple clients with a sever as controller. It was used dring the Artistik Rezo party in 42 as animation in the e1 cluster.

## Installation
### Linux
+ You will need thoses packages libtool autoconf cmake xorg-dev libglu1-mesa-dev libbsd-dev libudev-dev libjpeg-dev libopenal-dev libogg-dev libvorbis-dev libflac-dev opencl-headers ocl-icd-opencl-dev
+ and just `make` the project

### Mac OSX
+ You will need libtool, autoconf and cmake packages
+ and just `make` the project

### Windows
+ GL HF

## Usage
For the client:  
`./visualishader`  
For the server:  
`./visualishader --server configfile [configfile2 ...]`  

## Uniforms
name | description
--- | ---
iResolution | dynamic resolution of the window
iGlobalTime | time in sec and form the program startup
iChannel[0-7] | texture channels

## Images

### Before the party (testing)

![](https://image.noelshack.com/fichiers/2017/48/4/1512058471-dsc-0823-jpg.jpg)\
Server with the map of the cluster:\
![](https://image.noelshack.com/fichiers/2017/48/4/1512058471-dsc-0828-jpg.jpg)
![](https://image.noelshack.com/fichiers/2017/48/4/1512058471-dsc-0849-jpg.jpg)\
Kaleido block:\
![](https://image.noelshack.com/fichiers/2017/48/4/1512058471-dsc-0833-jpg.jpg)\
Weird block:\
![](https://image.noelshack.com/fichiers/2017/48/4/1512058471-dsc-0851-jpg.jpg)


### During the party

Raymarched block:\
![](https://image.noelshack.com/fichiers/2017/29/1/1500292196-rsz-p6270490.jpg)
![](https://image.noelshack.com/fichiers/2017/48/4/1512054299-1500292777-rsz-2p6270491.png)\
![](https://image.noelshack.com/fichiers/2017/29/1/1500292664-rsz-p6270493.jpg)
Tunnels block:\
![](https://image.noelshack.com/fichiers/2017/48/4/1512058473-dsc-0873-jpg.jpg)
![](https://image.noelshack.com/fichiers/2017/29/1/1500292346-capture-d-ecran-2017-07-17-a-13-48-26.png)
![](https://image.noelshack.com/fichiers/2017/48/4/1512058472-dsc-0871-jpg.jpg)\
2D Fractal block:\
![](https://image.noelshack.com/fichiers/2017/48/4/1512058473-dsc-0878-jpg.jpg)
