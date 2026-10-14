# operative0

## What

Operative0 is a prototype for a language built around extensibility, inspired by Kernel and Rebol.

Operative's goal is to explore the semantic reach of a language built around a minimal core.
The core lacks features like garbage collection and polymorphic dispatch,
aiming toward implementing these features in itself.

## Progress

These milestones represent the minimal acceptable state of Operative0

- [x] parsing
- [x] runtime environment foundations
- [ ] TCO
- [ ] builtin library
    - [ ] function definition
    - [ ] arithmetic
    - [ ] I/O
    - [ ] granular control flow
    - [ ] environment manipulation
    - [ ] C access
- [ ] file preface
- [ ] full metacircularity
    - [ ] preface dialecting
    - [ ] GC
    - [ ] polymorphism
    - [ ] continuations (might become builtins instead)

## Compatibility

Only tested in Debian 13 and Windows 11

## How do I use it?

Requirements:

- CMake 3.30 or higher
- C11-capable compiler

Otherwise, the standard CMake commands can be used. For example, in Debian:
`cmake -B cmake-build-debug -DCMAKE_BUILD_TYPE=Debug && cmake --build cmake-build-debug`

## A prototype?

If this is a prototype, what is the ideal version of it? Operative0 is a tree-walker,
while the non-v0 version would mostly involve tackling the challenges of a runtime in which
a minimal, low-level core makes sense, i.e., LLVM and AOT compilation. Operative0
aims to explore only the semantics of the language.