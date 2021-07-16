UGBA testing infrastructure
===========================

1. Introduction
---------------

`libugba`_ is a library for development of GBA games. It can be used to build
actual GBA game ROMs, but it can also target a regular PC so that it is easier
to debug the logic in the game. It is written in C.

This repository contains several examples of how to use each one of the
subsystems of the library. All examples are also used as unit tests, and they
are part of the automated regresion testing system of the library.

The PC builds of the tests are simply run on the host by passing the program a
Lua script with instructions. The GBA builds are run on `GiiBiiAdvance`_, which
has basic support for Lua scripts as well. Ideally, it would test on other
emulators. The PC builds have some parts of the emulator inside them to simulate
the GBA hardware, and it would be a good idea to test the code against a
different emulator to have a second opinion.

2. Dependencies
---------------

To generate PC executables:

- **GCC**, **Clang**, **MSVC** or another compiler supported by CMake.
- **CMake 3.15** or later
- **SDL2 2.0.7** or later (AudioStream support needed)
- **Grit** (either the one that comes with devkitPro, or standalone).
- **libpng 1.6** or later (Simplified API support needed): Needed to save PNGs
  like screenshots and dumps from VRAM debugger windows.
- **liblua 5.2** or later (Integer support needed): Needed for the Lua
  interpreter used for the unit tests.

To generate GBA ROMs:

- `devkitPro`_

You need to install devkitPro following the instructions in this link, then
follow the instructions in the sections below.

https://devkitpro.org/wiki/Getting_Started

The following projects are integrated as submodules in this repository, and they
are only used when building the examples, so you don't need to worry about them:

- `UMOD Player`_: For some audio tests.
- `GiiBiiAdvance`_: To run the GBA ROM version of the unit tests.

Note regarding Grit
^^^^^^^^^^^^^^^^^^^

If you don't install devkitPro because you only want to build the PC
executables, you still need to get Grit to convert the graphics into the right
format for the GBA: https://github.com/devkitPro/grit/releases

You'll need to make sure that CMake can find it by adding it to your system's
``PATH`` environment variable. If you have installed devkitPro, the build system
should be able to find the Grit executable installed by it. If you don't want to
have it in your ``PATH``, you can also set the ``GRIT_PATH`` variable when
invoking cmake: ``cmake .. -DGRIT_PATH=/path/to/grit/folder/``

3. Build PC library and examples
--------------------------------

Linux
^^^^^

If you're on Linux or any Linux-like environment (like MinGW or Cygwin), install
the dependencies using your package manager. For example, in Debian or Ubuntu:

.. code:: bash

    sudo apt install libsdl2-dev liblua5.4-dev libpng-dev

Clone this repository along with its submodules:

.. code:: bash

    git clone --recurse-submodules https://github.com/AntonioND/ugba-testing

Finally, go to the folder of **ugba**. The following will build the library,
examples, and run all the tests to verify it's working:

.. code:: bash

    mkdir build
    cd build
    cmake ..
    make -j`nproc`
    ctest

Windows
^^^^^^^

In order to build with **MinGW** or **Cygwin**, you should use the Linux
instructions. The following instructions have been tested with Microsoft Visual
C++ 2019.

You need to install `vcpkg`_. In short, open a **PowerShell** window and do:

.. code:: bash

    git clone https://github.com/Microsoft/vcpkg.git
    cd vcpkg
    .\bootstrap-vcpkg.bat
    .\vcpkg integrate install --triplet x64-windows

Then, install the dependencies (SDL2, libpng and liblua):

.. code:: bash

    .\vcpkg install SDL2 libpng liblua --triplet x64-windows

Clone this repository along with its submodules:

.. code:: bash

    git clone --recurse-submodules https://github.com/AntonioND/ugba-testing

Finally, go to the folder of **ugba**. The following will build the library,
and examples, in **Developer Command Prompt for VS 2019**:

.. code:: bash

    mkdir build
    cd build
    cmake .. -DCMAKE_TOOLCHAIN_FILE=C:\...\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows
    msbuild ugba.sln

In order to get a release build, do:

.. code:: bash

    msbuild ugba.sln /property:Configuration=Release

GBA
^^^

Clone this repository along with its submodules:

.. code:: bash

    git clone --recurse-submodules https://github.com/AntonioND/ugba-testing
    cd ugba

First, build **libugba**. Go to ``libugba`` and type ``make``.

Then, build the **UMOD Player**. Go to ``umod-player/player`` and type ``make``.

In order to build every one of the GBA examples and the PC examples:

.. code:: bash

    mkdir build
    cd build
    cmake .. -DBUILD_GBA=ON
    make -j`nproc`

Note: In order to make the compilation process faster you can run make in
multiple threads by doing ``make -j`nproc``.

In order to build every one of the GBA examples and the PC examples, and run all
the tests (both for PC and GBA), do:

.. code:: bash

    ctest

.. _GiiBiiAdvance: https://github.com/AntonioND/giibiiadvance
.. _UMOD Player: https://github.com/AntonioND/umod-player
.. _devkitPro: https://devkitpro.org/
.. _libugba: https://github.com/AntonioND/libugba
.. _vcpkg: https://github.com/microsoft/vcpkg
