# Artspeak : creative coding platform
**Use it at https://mrdaybird.github.io/artspeak/**

- At this point, it is basically a clone of [tixy.land](https://tixy.land/)

- In future, the idea to extend features and "try" to reach something like [shadertoy](https://www.shadertoy.com/), but staying minimal at its core.

## How?

- [x] For desktop -> write a language from scratch and use raylib for graphics
- [x] For web -> compile to html and wasm using emscripten
- [ ] Add more functions and operations

## Syntax

You have four read-only variables. 

- **t**: time
- **i**: index
- **x**: x-axis position
- **y**: y-axis position

You can write an expression with these four variables and provided math functions.

Supported functions and operations:
- sin, cos, tan.

Upcoming:
- power(**/^), abs, random, sqrt, and more...

## Using artspeak

For now, I have locally tested only on my machine(linux)

Dependencies: [fmt](https://fmt.dev/), [raylib](https://www.raylib.com/)

Then cmake into a build folder, hopefully it should work.

To build web version:
```
emcmake cmake -S . -B build-web -DPLATFORM=Web -G Ninja
cmake --build build-web
```
