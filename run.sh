#!/bin/sh

PWD="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

SFML="SFML/lib/"

export PATH=$PATH:$PWD/SFML/extlibs/libs-osx/Frameworks
export LD_LIBRARY_PATH=$PWD/$SFML:$DYLD_LIBRARY_PATH
echo $LD_LIBRARY_PATH

cd $PWD && ./visualishader $*
