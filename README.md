
Example Project: Robot Escape
=============================

This repository contains a minimalistic example game called **Robot Escape**, created to demonstrate how to use the [Erbsland Configuration Parser for C++](https://github.com/erbsland-dev/erbsland-cpp-configuration).

> 📘 *Note:* This project is intended as an educational example. To keep things simple, the code deliberately does not follow our usual coding style or best practices.

You can follow the full walkthrough here:  
[Erbsland Configuration Parser – Getting Started](https://cpp-configuration.erbsland.dev/chapters/get-started/)

How to Build and Run the Game
-----------------------------

This project uses CMake. To build the game, run the following commands:

```shell
cmake -B build .
cmake --build build
```

Once built, you can start the game by passing a configuration file as an argument:

```shell
./build/robot-escape/robot-escape configuration.elcl 
```

About This Repository
---------------------

The project is developed in incremental steps, each tagged as `v1`, `v2`, and so on. This makes it easy to explore how the project evolves over time. You can check out any version using its corresponding Git tag to follow the development step by step.

License
-------

**Robot Escape** ©2025 by <a href="https://erbsland.dev/">Tobias Erbsland / Erbsland DEV</a> is licensed under <a href="https://creativecommons.org/licenses/by/4.0/">CC BY 4.0</a><img src="https://mirrors.creativecommons.org/presskit/icons/cc.svg" alt="" style="max-width: 1em;max-height:1em;margin-left: .2em;"><img src="https://mirrors.creativecommons.org/presskit/icons/by.svg" alt="" style="max-width: 1em;max-height:1em;margin-left: .2em;">
