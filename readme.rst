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

To generate PC executables (and to run the tests):

- **GCC**, **Clang**, **MSVC** or another compiler supported by CMake.
- **CMake 3.15** or later
- **SDL2 2.0.7** or later (AudioStream support needed)
- **libpng 1.6** or later (Simplified API support needed): Needed to save PNGs
  like screenshots and dumps from VRAM debugger windows.
- **liblua 5.2** or later (Integer support needed): Needed for the Lua
  interpreter used for the unit tests.

To generate GBA ROMs you need one of:

- A ``gcc-arm-none-eabi`` toolchain: You can get it from your package manager,
  or from `Arm's GNU toolchain downloads website`_. In Ubuntu you can just do:

.. code:: bash

    sudo apt install gcc-arm-none-eabi

- `devkitPro`_: You need to install devkitPro following the instructions in the
  `devkitPro Getting Started guide`_, then follow the instructions in this
  readme. It comes with its own build of ``gcc-arm-none-eabi``.

The following projects are integrated as submodules in this repository, and they
are only used when building the examples, so you don't need to worry about them:

- `UMOD Player`_: For some audio tests.
- `GiiBiiAdvance`_: To run the GBA ROM version of the unit tests.
- `SuperFamiconv`_: Graphics converter.

3. Build and run examples on Linux
----------------------------------

Follow this if you're on Linux or any Linux-like environment (like MinGW or
Cygwin). First, clone this repository along with its submodules:

.. code:: bash

    git clone --recurse-submodules https://github.com/AntonioND/ugba-testing
    cd ugba-testing

Now, install the dependencies. For example, in Debian or Ubuntu:

.. code:: bash

    sudo apt install libsdl2-dev liblua5.4-dev libpng-dev

If you haven't installed **devkitPro** previously, use your package manager to
install the following package as well:

.. code:: bash

    sudo apt install gcc-arm-none-eabi

Now, convert the assets by running:

.. code:: bash

    bash assets.sh

The following will build the library, and examples for the host and for GBA:

.. code:: bash

    mkdir build
    cd build
    cmake .. -DBUILD_GBA=ON -DCMAKE_BUILD_TYPE=Release
    make -j`nproc`

Note: If you aren't using **devkitPro**, you need to add ``-DUSE_DEVKITARM=OFF``
to the ``cmake`` command.

To run the tests, simply do:

.. code:: bash

    ctest

4. Known bugs
-------------

- Audio tests on the GBA are too unreliable. All the test does is compare the
  waveform outputted by the emulator with a reference, so any small change in
  almost any part of the boot or audio code will break the test.

- Tests that use sprites only work in release builds. It is related to GCC
  generating byte writes to OAM when modifying 16-bit fields. This GCC bug seems
  to still be present, and preventing a trivial fix of just flagging all fields
  as volatile. https://gcc.gnu.org/bugzilla/show_bug.cgi?id=50521


.. _Arm's GNU toolchain downloads website: https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads
.. _devkitPro Getting Started guide: https://devkitpro.org/wiki/Getting_Started
.. _GiiBiiAdvance: https://github.com/AntonioND/giibiiadvance
.. _UMOD Player: https://github.com/AntonioND/umod-player
.. _SuperFamiconv: https://github.com/Optiroc/SuperFamiconv
.. _devkitPro: https://devkitpro.org/
.. _libugba: https://github.com/AntonioND/libugba
