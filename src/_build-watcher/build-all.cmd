@echo off
del libomapi

docker buildx build --output type=local,dest=./ .

dir /b libomapi

pause
