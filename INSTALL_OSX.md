Instructions for installing Natron from sources on OS X
=======================================================

These are step-by-step instructions to compile Natron on OS X.

OS X 10.6 (a.k.a. Snow Leopard) and newer are supported when building with MacPorts, and Homebrew can be used to compile it on the latest OS X.

## Checkout sources

    git clone https://github.com/MrKepzie/Natron.git
    cd Natron

If you want to compile the bleeding edge version, use the master
branch:

    git checkout master

Update the submodules:

    git submodule update -i --recursive

## Install libraries

In order to have Natron compiling, first you need to install the required libraries.

There are two exclusive options: using MacPorts or using Homebrew.

Homebrew is easier to set up than MacPorts, but cannot build universal binaries.

### MacPorts

You need an up to date MacPorts version. Just download it and install it from <http://www.macports.org>, and execute the following commands in a terminal:

    sudo port selfupdate
    sudo port upgrade outdated

Then, you should add the "ports" provided by Natron. Edit as root the file `/opt/local/etc/macports/sources.conf` (as in `sudo nano /opt/local/etc/macports/sources.conf`) and add the following line at the beginning, with the path to the Natron sources (yes, there are three slashes after `file:`):

    file:///Users/your_username/path_to_sources/Natron/tools/MacPorts

Then, create the index file:

    (cd /Users/your_username/path_to_sources/Natron/tools/MacPorts; portindex)

It is also recommended to add the  following line to `/opt/local/etc/macports/variants.conf`:

    -x11 +no_x11 +bash_completion +no_gnome +quartz

If compiling on Mac OS X 10.6 with Xcode 4 (using GCC 4.2.1 and libstdc++), you should also revert to the default compilers list of Xcode 3.2.6 (MacPort's `/opt/local/libexec/macports/lib/port1.0/portconfigure.tcl` sets it to a different value for an unknown reason, resulting in llvm-gcc-4.2 being used to compile everything in MacPorts). Add the following line to `/opt/local/etc/macports/macports.conf`:

    default_compilers gcc-4.2 clang llvm-gcc-4.2 macports-clang-3.4 macports-clang-3.3 macports-llvm-gcc-4.2 apple-gcc-4.2 gcc-4.0

Now, if you want to use turbojpeg instead of jpeg:
    sudo port -f uninstall jpeg
    sudo port -v install libjpeg-turbo
    
And finally install the required packages:

    sudo port -v install qt4-mac boost cairo expat gsed py27-pyside py27-sphinx
    sudo ln -s python2.7-config /opt/local/bin/python2-config

Create the file /opt/local/lib/pkgconfig/glu.pc containing GLU
configuration, for example using the following comands:

```Shell
sudo -s
cat > /opt/local/lib/pkgconfig/glu.pc << EOF
 prefix=/usr
 exec_prefix=${prefix}
 libdir=${exec_prefix}/lib
 includedir=${prefix}/include


Name: glu
 Version: 2.0
 Description: glu
 Requires:
 Libs:
 Cflags: -I${includedir}
EOF
```

If you intend to build the [openfx-io](https://github.com/MrKepzie/openfx-io) plugins too, you will need these additional packages:

    sudo port -v install openexr ffmpeg opencolorio openimageio +natron seexpr

and for [openfx-arena](https://github.com/olear/openfx-arena) (note that it installs a version of ImageMagick without support for many image I/O libraries):

    sudo port -v install librsvg ImageMagick +natron poppler librevenge libcdr-0.1 libzip

### Homebrew

Install homebrew from <http://brew.sh/>

macOS 10.12 and later: Qt 4 is not supported in homebrew on masOS 10.12 and later, please enable the community-maintained recipe using:

    brew tap cartr/qt4

Patch the qt4 recipe to fix the stack overflow issue (see [QTBUG-49607](https://bugreports.qt.io/browse/QTBUG-49607), [homebrew issue #46307](https://github.com/Homebrew/homebrew/issues/46307), [MacPorts ticket 49793](http://trac.macports.org/ticket/49793)).

Patching a homebrew recipe is explained in the [homebrew FAQ](https://github.com/Homebrew/homebrew/blob/master/share/doc/homebrew/FAQ.md).

    brew edit qt (on macOS 10.12 and later: brew edit cartr/qt4/qt)

and before the line that starts with `head`, add the following code:

      patch :p0 do
        url "https://bugreports.qt.io/secure/attachment/52520/patch-qthread-stacksize.diff"
        sha256 "477630235eb5ee34ed04e33f156625d1724da290e7a66b745611fb49d17f1347"
      end


Install libraries:

    brew tap homebrew/python
    brew tap homebrew/science
    brew install expat cairo gnu-sed
    brew install --build-from-source qt --with-mysql (on 

On macOS Sierra, install the sierra-compatible recipe (to be used only in Sierra, since this builds Qt from sources and takes a while):

    brew install --build-from-source cartr/qt4/qt --with-mysql

Then install pyside (the boneyard tap is for pyside, which does not yet build with Qt5 and was thus removed from the homebrew core):

    brew tap homebrew/boneyard
    brew install pyside sphinx-doc

The last command above will take a while, since it builds from sources, and should finally tell you do do the following if the `homebrew.pth` file does not exist:

    mkdir -p /Users/devernay/Library/Python/2.7/lib/python/site-packages
    echo 'import site; site.addsitedir("/usr/local/lib/python2.7/site-packages")' >> ~/Library/Python/2.7/lib/python/site-packages/homebrew.pth
    sudo ln -s ~/Library/Python/2.7/lib/python/site-packages/homebrew.pth /Library/Python/2.7/lib/python/site-packages/homebrew.pth

 To install the openfx-io and openfx-misc sets of plugin, you also need the following:

    brew install ilmbase openexr freetype fontconfig ffmpeg opencolorio openimageio seexpr

To install the openfx-arena set of plugin, you also need the following:

    brew install librsvg poppler librevenge libcdr libzip
    brew uninstall imagemagick
    brew install imagemagick --with-hdri --with-librsvg --with-quantum-depth-32 --with-pango


also set the correct value for the pkg-config path (you can also put
this in your .bash_profile):

    export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:/opt/X11/lib/pkgconfig:/usr/local/opt/cairo/lib/pkgconfig:/usr/local/opt/icu4c/lib/pkgconfig

### Installing manually (outside of MacPorts or homebrew) a patched Qt to avoid stack overflows

The following is not necessary if a patched Qt was already installed by the standard MacPorts or homebrew procedures above.

See [QTBUG-49607](https://bugreports.qt.io/browse/QTBUG-49607), [homebrew issue #46307](https://github.com/Homebrew/homebrew/issues/46307), [MacPorts ticket 49793](http://trac.macports.org/ticket/49793).

    wget https://download.qt.io/official_releases/qt/4.8/4.8.7/qt-everywhere-opensource-src-4.8.7.tar.gz
    tar zxvf qt-everywhere-opensource-src-4.8.7.tar.gz
    cd qt-everywhere-opensource-src-4.8.7
    wget https://raw.githubusercontent.com/Homebrew/patches/480b7142c4e2ae07de6028f672695eb927a34875/qt/el-capitan.patch
    patch -p1 < el-capitan.patch
    wget https://bugreports.qt.io/secure/attachment/52520/patch-qthread-stacksize.diff
    patch -p0 < patch-qthread-stacksize.diff

Then, configure using:

    ./configure -prefix /opt/qt4 -pch -system-zlib -qt-libtiff -qt-libpng -qt-libjpeg -confirm-license -opensource -nomake demos -nomake examples -nomake docs -cocoa -fast -release

* On OS X >= 10.9 add `-platform unsupported/macx-clang-libc++`
* On OS X < 10.9, to compile with clang add `-platform unsupported/macx-clang`
* If compiling with gcc/g++, make sure that `g++ --version`refers to Apple's gcc >= 4.2 or clang >= 318.0.61
* To use another openssl than the system (mainly for security reasons), add `-openssl-linked -I /usr/local/Cellar/openssl/1.0.2d_1/include -L /usr/local/Cellar/openssl/1.0.2d_1/lib` (where the path is changed to your openssl installation).

Then, compile using:

    make

And install (after making sure `/opt/qt4` is user-writable) using:

    make install


### Download OpenColorIO-Configs

In the past, OCIO configs were a submodule, though due to the size of the repository, we have chosen instead
to make a tarball release and let you download it [here](https://github.com/MrKepzie/OpenColorIO-Configs/archive/Natron-v2.1.tar.gz).
Place it at the root of Natron source tree:

    curl -k -L https://github.com/MrKepzie/OpenColorIO-Configs/archive/Natron-v2.1.tar.gz | tar zxf -
    mv OpenColorIO-Configs-Natron-v2.1 OpenColorIO-Configs


## Add the config.pri file

You have to define the locations of the required libraries.
This is done by creating a .pri file that will tell the .pro where to find those libraries.
The only library to put in the config.pri file on unix systems is boost.
For all other libraries are found with PKGConfig.

- create the config.pri file next to the Project.pro file.

You can fill it with the following proposed code to point to the libraries.
Of course you need to provide valid paths that are valid on your system.

INCLUDEPATH is the path to the include files

LIBS is the path to the libs

If you installed libraries using MacPorts, use the following
config.pri:

```Shell
 # copy and paste the following in a terminal
cat > config.pri << EOF
boost: INCLUDEPATH += /opt/local/include
boost: LIBS += -L/opt/local/lib -lboost_serialization-mt
EOF
```

If you installed libraries using Homebrew, use the following
config.pri:

```Shell
 # copy and paste the following in a terminal
cat > config.pri << EOF
boost: INCLUDEPATH += /usr/local/include
boost: LIBS += -L/usr/local/lib -lboost_serialization-mt -lboost_thread-mt -lboost_system-mt
expat: PKGCONFIG -= expat
expat: INCLUDEPATH += /usr/local/opt/expat/include
expat: LIBS += -L/usr/local/opt/expat/lib -lexpat
EOF
```

## Build with Makefile

You can generate a makefile by opening a Terminal, setting the current
directory to the toplevel source directory, and typing

    qmake -r

then type

    make

This will create all binaries in all the subprojects folders.

If you want to build in DEBUG mode change the qmake call to this line:

    qmake -r CONFIG+=debug

* You can also enable logging by adding CONFIG+=log

* You can also enable clang sanitizer by adding CONFIG+=sanitizer

### Building with OpenMP support using clang

It is possible to build Natron using clang (version 3.8 is required,
version 3.9.1 is recommended) with OpenMP support on
MacPorts (or homebrew for OS X 10.9 or later).  OpenMP brings speed improvements in the
tracker and in CImg-based plugins.

However, the unit tests don't pass yet with clang/libomp 3.9.1 on OS X 10.6, so be sure tu run the unit tests from https://github.com/MrKepzie/Natron-Tests to validate any clang/macOS combination.

First, install clang 3.9. On OS X 10.9 and later, simply execute:

    sudo port -v install clang-3.9

On older systems, follow the procedure described in "[https://trac.macports.org/wiki/LibcxxOnOlderSystems](Using libc++ on older system)". Note that we noticed clang 3.9.1 generates wrong code with `-Os` when compiling openexr, so it is safer to also change `default configure.optflags      {-Os}` to `default configure.optflags      {-O2}` in `/opt/local/libexec/macports/lib/port1.0/portconfigure.tcl` (type `sudo nano /opt/local/libexec/macports/lib/port1.0/portconfigure.tcl` to edit it).

Then, configure using the following qmake command:

    /opt/local/libexec/qt4/bin/qmake -spec unsupported/macx-clang-libc++ QMAKE_CXX=clang++-mp-3.9 QMAKE_CXX=clang++-mp-3.9 QMAKE_CC=clang-mp-3.9 QMAKE_OBJECTIVE_CXX=clang++-mp-3.9 QMAKE_OBJECTIVE_CC=clang-mp-3.9 QMAKE_LD=clang++-mp-3.9 -r CONFIG+=openmp

To build the plugins, use the following command-line:

    make CXX=clang++-mp-3.9 CXXFLAGS_ADD="-fopenmp" LDFLAGS_ADD="-fopenmp"

Or, if you have MangledOSMesa32 installed in `OSMESA_PATH` and LLVM installed in `LLVM_PATH` (MangledOSMesa32 and LLVM build script is available from [https://github.com/devernay/osmesa-install](github:devernay/osmesa-install) :

    OSMESA_PATH=/opt/osmesa
    LLVM_PATH=/opt/llvm
    make CXX=clang++-mp-3.9 CXXFLAGS_ADD="-fopenmp" LDFLAGS_ADD="-fopenmp" CXXFLAGS_MESA="-DHAVE_OSMESA" LDFLAGS_MESA="-L${OSMESA_PATH}/lib -lMangledOSMesa32 `${LLVM_PATH}/bin/llvm-config --ldflags --libs engine mcjit mcdisassembler | tr '\n' ' '`" OSMESA_PATH="${OSMESA_PATH}"

## Build on Xcode

Follow the instruction of build but
add -spec macx-xcode to the qmake call command:

    qmake -r -spec macx-xcode

Then open the already provided Project-xcode.xcodeproj and compile the target "all"

### Compiling plugins with Xcode

The source distributions of the plugin sets `openfx-io` and
`openfx-misc` contain Xcode projects, but these require setting a few
global variables in Xcode. These variables can be used to switch
between the system-installed version of a package and a custom install
(e.g. if you need to debug something that happens in OpenImageIO).

In Xcode Preferences, select "Locations", then "Source Trees", and add the following
variable names/values (Xcode may need to be restarted after setting these):
- `LOCAL`: `/usr/local` on Homebrew, `/opt/local` on MacPorts
- `BOOST_PATH`: `$(LOCAL)/include`
- `EXR_PATH`: `$(LOCAL)`
- `FFMPEG_PATH`: `$(LOCAL)`
- `OCIO_PATH`: `$(LOCAL)`
- `OIIO_PATH`: `$(LOCAL)`
- `OPENCV_PATH`: `$(LOCAL)`
- `SEEXPR_PATH`: `$(LOCAL)`

It is also recommended in Xcode Preferences, select "Locations", then
"Locations", to set the Derived Data location to be Relative, and in
the advanced settings to set the build location to Legacy (if not,
build files are somewhere under `~/Library/Developer/Xcode`.

### Xcode caveats

Whenever the .pro files change, Xcode will try to launch qmake and
probably fail because it doesn't find the necessary binaries (qmake,
moc, pkg-config, python3-config, etc.). In this case, just open a
Terminal and relaunch the above command. This will rebuild the Xcode projects.

Alternatively, you can globally add the necessary directories
(`/usr/local/bin`on Homebrew, `/opt/local/bin` on MacPorts) to you
PATH (see <http://www.emacswiki.org/emacs/EmacsApp> for instructions).

On MacPorts, this would look like:

    launchctl setenv PATH /opt/local/bin:/opt/local/sbin:/usr/bin:/bin:/usr/sbin:/sbin

## Testing

    (cd Tests && qmake -r CONFIG+=debug CONFIG+=coverage && make -j4 && ./Tests)

## Generating Python bindings

This is not required as generated files are already in the repository. You would need to run it if you were to extend or modify the Python bindings via the
typesystem.xml file. See the documentation of shiboken-3.4 for an explanation of the command line arguments.

On MacPorts:
```Shell
rm Engine/NatronEngine/* Gui/NatronGui/*

shiboken-3.4 --avoid-protected-hack --enable-pyside-extensions --include-paths=../Engine:../Global:/opt/local/include:/opt/local/include/PySide-3.4  --typesystem-paths=/opt/local/share/PySide-3.4/typesystems --output-directory=Engine Engine/Pyside_Engine_Python.h  Engine/typesystem_engine.xml

shiboken-3.4 --avoid-protected-hack --enable-pyside-extensions --include-paths=../Engine:../Gui:../Global:/opt/local/include:/opt/local/include/PySide-3.4  --typesystem-paths=/opt/local/share/PySide-3.4/typesystems:Engine --output-directory=Gui Gui/Pyside_Gui_Python.h  Gui/typesystem_natronGui.xml

tools/utils/runPostShiboken.sh
```

on HomeBrew:
```Shell
rm Engine/NatronEngine/* Gui/NatronGui/*

shiboken --avoid-protected-hack --enable-pyside-extensions --include-paths=../Engine:../Global:/usr/local/include:/usr/local/include/PySide  --typesystem-paths=/usr/local/share/PySide/typesystems --output-directory=Engine Engine/Pyside_Engine_Python.h  Engine/typesystem_engine.xml

shiboken --avoid-protected-hack --enable-pyside-extensions --include-paths=../Engine:../Gui:../Global:/usr/local/include:/usr/local/include/PySide  --typesystem-paths=/usr/local/share/PySide/typesystems:Engine --output-directory=Gui Gui/Pyside_Gui_Python.h  Gui/typesystem_natronGui.xml

tools/utils/runPostShiboken.sh
```

**Note**
Shiboken has a few glitches which needs fixing with some sed commands, run tools/utils/runPostShiboken.sh once shiboken is called

## OpenFX plugins

Instructions to build the [openfx-io](https://github.com/MrKepzie/openfx-io) and [openfx-misc](https://github.com/devernay/openfx-misc) sets of plugins can also be found in the [tools/packageOSX.sh](https://github.com/MrKepzie/Natron/blob/master/tools/packageOSX.sh) script if you are using MacPorts, or in the .travis.yml file in their respective github repositories if you are using homebrew ([openfx-misc/.travis.yml](https://github.com/devernay/openfx-misc/blob/master/.travis.yml), [openfx-io/.travis.yml](https://github.com/MrKepzie/openfx-io/blob/master/.travis.yml).


You can install [TuttleOFX](http://www.tuttleofx.org/) using homebrew:

    brew tap homebrew/science homebrew/x11 homebrew/python cbenhagen/video
    brew install tuttleofx


Or have a look at the [instructions for building on MacPorts as well as precompiled universal binaries](http://devernay.free.fr/hacks/openfx/#OSXTuttleOFX).
