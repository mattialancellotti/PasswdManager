README
------

Simple _password generator_ created to practice.

> **Passwd Generator**
>	-l, -length: to specify the length of the password.
>	-n, -not-admitted: to specify which type of character is not allowed [digit - u\_char - l\_char - sign]
>	-t, -times: to specify how many password you need
>	-s, -save: to save all the passwords generated in a file
>	-u, -use: to specify which password you want to use [1 - 2 - 3...]  (usage: -use 1,2,3)
>	-p, -print: to print the stats
>	-h, -help: to show this message
>	-v, -version: to print the current version of the program

Compilation:
> gcc -Wall -Werror -Wextra -Wpedantic -ggdb main.c main.h utils.c utils.h mem.h args.h
