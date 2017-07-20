# THE_FUTURE

Network program to render concurently shaders on multiple clients.

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

![](https://image.noelshack.com/fichiers/2017/29/1/1500292196-rsz-p6270490.jpg)
![](https://image.noelshack.com/fichiers/2017/29/1/1500292346-capture-d-ecran-2017-07-17-a-13-48-26.png)
![](https://image.noelshack.com/fichiers/2017/29/1/1500292777-rsz-2p6270491.jpg)
![](https://image.noelshack.com/fichiers/2017/29/1/1500292664-rsz-p6270493.jpg)
