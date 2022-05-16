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

If some images are cutted due to github image caching system, you can [check them out here](https://infotechfrance-my.sharepoint.com/:f:/g/personal/a_lelievre_infotech_fr/EisItCWaV-pKi1nLveC5i3IB64-Cx7dFNOXwaS4mBh4kIQ?e=UnAmjO).


### Before the party (testing)
![fad34742-dcbe-45a2-b4d4-9c83e6b839d1](https://user-images.githubusercontent.com/6877923/168647957-5c4194dd-6a88-447a-b3f4-10732415b6c5.jpg)

Server with the map of the cluster:
![7d6ce02b-ac20-4ba2-9df9-929cc95a531d](https://user-images.githubusercontent.com/6877923/168647999-d971b5d4-3f23-4717-af52-f3d11699ed2b.jpg)

Kaleido block:
![224233df-99fa-4cb9-801a-56bd57cbed63](https://user-images.githubusercontent.com/6877923/168648443-d64eaadf-f885-452e-b1f4-2e42e4a2dee6.jpg)


### During the party

Raymarched block:
![8a45455b-2f44-482f-aca7-2516fa239239](https://user-images.githubusercontent.com/6877923/168647928-3c53a257-a8be-4463-b8c9-9e53bd480d5f.jpg)

Tunnels block:
![f0cb3e35-5210-4ccc-824c-3cf9d0e55009](https://user-images.githubusercontent.com/6877923/168648101-c290a21d-883f-4738-9a3a-be4604e5f42d.jpg)

2D Fractal block:
![eee010de-d445-4aab-94ab-2c592b5a913e](https://user-images.githubusercontent.com/6877923/168648050-3fe8caee-be47-4861-ac65-6b3b30289d3a.jpg)
![876340a2-b69a-4032-8ebd-df51d0a90d40](https://user-images.githubusercontent.com/6877923/168648176-23dce223-467e-4208-b2cd-e0e9ad8aae11.jpg)

Others:
![46daf591-14c4-4222-93ca-5f1d64ebdcdd](https://user-images.githubusercontent.com/6877923/168648346-ab96350a-7251-4bb7-b6ec-7f2c03b0a30e.jpg)

