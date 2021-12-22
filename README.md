README
------

## PasswdManager - ezpass
### Introduction
This objective of this project is to develop an easy to use password manager
for personal use. Previously this project was a password generator, that's why
this program can also generate passwords whith different characteristics.

### Libraries
+ libsodium

### Testing
This program can't be used yet (it's missing the encrpyting part) but can be
tested. There are different methods to test it:
+ To compile it the same git repo you downloaded just run `make`;
+ To compile it and install it in $PATH run `make stow`, and to uninstall it run
  `make unstow`;
+ Finally if you don't want to mess around with your system (or just like
  docker) run `make docker`.

To clean everything up run `make clean`.
