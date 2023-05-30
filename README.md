# 2023 1st Semester Introduction to Programming(1) term project

## Summary

The topic of last term project was "Lotto 6/45 simulator".

I must write following features:
- 1. single game mode (using keyboard input)
- 2. single game mode (using file input)
- 3. multi game simulation mode

Also I decided to write additional features to provide more convenience experience:
- 4. most secure random number generator `ChaCha20`
- 5. console abstraction

## Build

### Pre-requirements
- OS: any OSes where POSIX is supported (including Microsoft Windows by MinGw toolchain)
- Software: cmake, any C compiler which supports C11 standard with `threads.h` (eg. GCC, Clang)

### Instruction

1. Clone this repository then enter the directory of this repository in any console.

2. Follow this:

```Bash
$ mkdir build
$ cd build
$ cmake ..
$ make
$ ./lotto-645-simulator
```

## License

This software is distributed under the MIT license.

You can check terms of this license [here](./LICENSE).