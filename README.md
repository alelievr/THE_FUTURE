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

## Developed notions
+ Server:
+ + Graphic:
+ + + Cluster control interface with status display for each Imacs.
+ + + Change render group parameters from this interface (sync, render move, displayed shaders ...).
+ + configuration:
+ + + Config directory to specify the render group of each Imacs.
+ + + Simple scripting language for the render loop of each groups.

+ + Networking:
+ + + Cluster scan to find all connected clients.
+ + + render group synchronization with local computed timestamp.
+ + + Debug tool to check the IP and position of each Imacs.

+ client:
+ + Graphic : 
+ + + Renderbuffers OpenGl
+ + + Volumetric lighting
+ + + Raymarching based coloring (AO, blackbody, lighting ...)
+ + + Post processing (bloom, ...)
+ + + 2D fractals (newton, ribbon, ...)
+ + + Fractale 3D (Mandelbox)
+ + + Real time on GTX 660M (mac)
+ + + sharing context OpenGl/OpenCl
+ + + IFS OpenCl
+ + Networking: 
+ + + Simple communication protocol for server-client communication (UDP based)
+ + + Asynchronous server commands (never block the main thead) 
+ + + Cluster scan to find a server else, wait for a server
+ + + Hot reload for shaders and parameters (uniforms)
+ + other:
+ + + Deployment script to target each Imacs of the cluster with one command.


## Images

If some images are cutted due to github image cahcing system, you can [check them out here](https://infotechfrance-my.sharepoint.com/:f:/g/personal/a_lelievre_infotech_fr/EisItCWaV-pKi1nLveC5i3IB64-Cx7dFNOXwaS4mBh4kIQ?e=UnAmjO).

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
Tunnels block:\
![](https://image.noelshack.com/fichiers/2017/48/4/1512058473-dsc-0873-jpg.jpg)
![](https://image.noelshack.com/fichiers/2017/29/1/1500292346-capture-d-ecran-2017-07-17-a-13-48-26.png)
![](https://image.noelshack.com/fichiers/2017/48/4/1512058472-dsc-0871-jpg.jpg)\
2D Fractal block:\
![](https://image.noelshack.com/fichiers/2017/48/4/1512058473-dsc-0878-jpg.jpg)
