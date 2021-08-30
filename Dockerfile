FROM alpine

# Installing the app and its runtime/build dependencies
RUN apk add git stow make clang libc-dev gcc

# Preparing the build environment
RUN mkdir -p /home/ezpass
RUN git clone -b master https://github.com/mattialancellotti/PasswdManager.git /home/ezpass
WORKDIR /home/ezpass

RUN make && make stow

ENTRYPOINT [ "ezpass" ]
