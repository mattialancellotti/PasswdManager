FROM ubuntu:latest

# Installing the app and its runtime/build dependencies
ENV DEBIAN_FRONTEND noninteractive
ENV HOME /home
RUN apt-get update
RUN apt-get --yes install build-essential git libsodium-dev clang stow

# Preparing the build environment
RUN useradd -b /home -m -f 0 -s /bin/bash ezpass
RUN mkdir -p /home/ezpass/.local/share
RUN chown -R ezpass /home/ezpass/.local/share
RUN git clone -b fix_docker_mclose https://github.com/mattialancellotti/PasswdManager.git /home/ezpass/app/

WORKDIR /home/ezpass/app
RUN make EXPERIMENTAL=yes SODIUM=yes DEBUG=yes -B && make stow

USER ezpass
ENTRYPOINT [ "/bin/bash" ]
