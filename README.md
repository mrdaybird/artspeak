# Artspeak : creative coding platform
**Use it at https://mrdaybird.github.io/artspeak/**

## What?

- At this point, it is basically a clone of [tixy.land](https://tixy.land/)
- In future, the idea to extend features and "try" to reach something like [shadertoy](https://www.shadertoy.com/), but staying minimal at its core.
- But unlike *tixy.land*, it ships its **own language and interpreter**! What does this mean?
	1. This means it is not dependent on the browser and javascript. 
	2. I have full control on langauge design and features.
	3. Better performance. (potentially?) Though, it does not mean much for *now!*

## Syntax

You have four read-only variables. 

- **t**: time
- **i**: index
- **x**: x-axis position
- **y**: y-axis position

You can write an expression with these four variables and provided math functions.

Supported functions and operations:
- sin, cos, tan, sqrt

Upcoming:
- power(**/^), abs, random, and more...

## How? 

The language at its core is a simple bytecode interpreter written in C++.
Thus each time you write a expression, it is compiled to stack-based bytecode, which is then evaluated on given time value and *i*,*x* and *y* attributes of each *canvas* element. (*Canvas* here refers to the animation/graphics that you see.)
The canvas is created using raylib. 
All this is made available to the web using emscripten!

**Progress:**

- [x] For desktop -> write a language from scratch and use raylib for graphics
- [x] For web -> compile to javascript and wasm using emscripten
- [ ] Add more functions and operations

## Using artspeak

**(to be updated)**

For now, I have locally tested only on my machine(linux)

Dependencies: [fmt](https://fmt.dev/), [raylib](https://www.raylib.com/)

Then cmake into a build folder, hopefully it should work.

To build web version:
```
emcmake cmake -S . -B build-web -DPLATFORM=Web -G Ninja
cmake --build build-web
```
