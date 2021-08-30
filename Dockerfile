FROM ubuntu:latest

# Installing the app and its runtime/build dependencies
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update
RUN apt-get --yes install build-essential git libsodium-dev clang stow

# Preparing the build environment
RUN mkdir -p /home/ezpass
RUN mkdir -p /home/.local/share
RUN git clone -b master https://github.com/mattialancellotti/PasswdManager.git /home/ezpass
WORKDIR /home/ezpass

RUN make EXPERIMENTAL=yes SODIUM=yes && make stow

ENTRYPOINT [ "ezpass" ]
