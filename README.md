# MultiRename
An implementation of a rename tool for multiple files for Amiga 
computers using the Reaction gui toolkit. Because of *Reaction* you need
at least *AmigaOS 3.1.4* to use it.

# Build and debug

## Amiga

On the Amiga you need *OS3.2.2* with a fully installed *SAS-C 6.5* and
[Codecraft](http://boemann.dk/codecraft/) development environment.

Open Codecraft, open the project file `MultiRename.projecttree`, use the
Build menu to build, then hit `F5` to run the app in the debugger.

## Linux
### Dependencies
The project is tested to build with Debian 12 bookworm on Windows 11 WSL
with the following packages installed:

 - build-essentials
 - cmake
 - git
 - [ Bebbos gcc 6.5 toolchain](https://github.com/bebbo/amiga-gcc) 
which is expected to be installed in /opt

#### reaction.lib

This lib must be manually copied from

    /opt/amiga/m68k-amigaos/vbcc/lib/reaction.lib

to

    opt/amiga/lib/libreaction.a

(note the renaming) to make it work.

### Build
To build this project a Makefile must be created with cmake:

- Manually create a directory *build-gcc* next to the *src* directory

- Enter this directory and type.
<!-- -->
    cmake -DCMAKE_BUILD_TYPE=Release ..


- For preparing a debug Makefile with debug information, type:
<!-- -->
    cmake -DCMAKE_BUILD_TYPE=Debug ..

Then cmake is configured. Build can be started by simply typing 
    
    make 

from inside the build directory.

After the build was successful the binary *Pooyan_gcc* is copied into
the project root directory.

## VSCode integration
The VSCode default build task (see tasks.json) has been updated to use
the *cmake* generated Makefile inside the *build* directory.

So, after cmake is prepared for debug or release (see above), building
can be started with *Ctrl + Shift + b* from within VSCode.

### Debug the RenameParser test with Linux

To debug the RenameParser test in the `scr/_tests` directory with Linux,
the cmake run, see above, must've been done with the
`-DCMAKE_BUILD_TYPE=Debug` option. Only then the breakpoints you set in
VSCode will be hit.

With this done properly the `test_action_parser.c` entry point can be
build and run in debug mode by simply hitting `F5` in VScode.