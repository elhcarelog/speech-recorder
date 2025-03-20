#!/bin/bash

set -e
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
pushd "$HERE" &>/dev/null

if [[ -z $1 ]]; then
	echo "Usage: setup.sh x86|x64|arm64"
	exit 1
fi

rm -rf tmp lib/3rd_party/portaudio

mkdir -p tmp/portaudio
cd tmp/portaudio
curl -Lo portaudio.tgz http://files.portaudio.com/archives/pa_stable_v190700_20210406.tgz
tar xvf portaudio.tgz

cd portaudio
mkdir dist install
cd dist

portaudio_cmake="cmake"
if [[ $(uname -s) == "MINGW"* ]]; then
	if [[ $1 == "x86" ]]; then
		portaudio_cmake+=" -A Win32"
	elif [[ $1 == "x64" ]]; then
		portaudio_cmake+=" -A x64"
	fi
elif [[ $(uname -s) == "Darwin" ]]; then
	portaudio_cmake+=" -DCMAKE_OSX_DEPLOYMENT_TARGET=10.14"
	if [[ $1 == "x64" ]]; then
		portaudio_cmake+=" -DCMAKE_OSX_ARCHITECTURES=x86_64"
	elif [[ $1 == "arm64" ]]; then
		portaudio_cmake+=" -DCMAKE_OSX_ARCHITECTURES=arm64"
	fi
fi

portaudio_cmake+=" .."
eval $portaudio_cmake
cmake --build . --config Release
cmake --install . --prefix ../install
cp -r ../install ../../../../lib/3rd_party/portaudio

cd ../../..
rm -rf tmp
popd &>/dev/null
