# Multithreaded JAYA in C
## Building:
- simply use `make` to build the project
- `make debug` for debug mode (print fitness progression to file, debuggable binary)
- `make clean` to remove all build files.
- `make profile` to analyze runtime + function calls (good to check maxFEs).
## Usage:
```
./jaya pop_size dimensions maxFE function
```