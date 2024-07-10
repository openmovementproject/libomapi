#!/bin/sh

rm libomapi

docker buildx build --output type=local,dest=./ .

#if [[ "$OSTYPE" == "darwin"* ]]; then   lipo -create -output watcher-mac watcher-mac-x64 watcher-mac-arm   ; fi

ls libomapi
