# Progress Log
This log will be used to track what I am doing, different methods that I try and used to save links and other resources that I use. 
## Obtaining data from Wikipedia
Wikipedia provides lots of different data formats to download data. However the site is very confusing to use. I eventually found a way to downlaod just a current snapshot of all english pages which takes up around 16GB

1) Downloaded the XML dump files from https://dumps.wikimedia.org/enwiki/20240401/ (pages articles multistream)
2) Verified files against checksum to ensure nothing is corrupt 
3) Uncompress the bz2 file which took around half an hour. It expaneded to a size of 100GB
3) Need to find a way to extract information from the giant xml file.

## Selecting a suitable XML parser

- I initally selected [PUGIXML](https://pugixml.org/) as the XML parser of choice due to it being very small, simple, and well documented. However, after doing further research, I found that using a DOM based parser would not be suitable due to the size of the files. and instead a SAX based parser is required.
- Instead, I will use libxml2 which is a GNOME project. [Libxml++](https://libxmlplusplus.sourceforge.net/) is a C++ wrapper for libxml2 and this will be used as I am going to develop this project in C++. 

## Setting up a Package Manager and Build System
- After attempting to build Libxml++ and implement it into my project with CMake and getting nowhere, I decided to try to use a package manager.
- The Libxml++ documentation website returns a 404 error however the documentation can be built from the release tar.

### Conan
- I initially selected Conan as the package manager for this project and followed [this tutorial](https://docs.conan.io/2/tutorial/consuming_packages/build_simple_cmake_project.html) to set it up. The package page for libxmlpp on Conan can be found [here](https://conan.io/center/recipes/libxmlpp?version=5.2.0) and the source code can be found [here](https://github.com/libxmlplusplus/libxmlplusplus?tab=readme-ov-file).
- I am unable to get libxml++ to link to the project. However, it does not seem to be an issue of Conan as that is able to find and download the package properly.

### Vcpkg
- An alternative method is to use Vcpkg instead of Conan so I removed Conan from the project and installed Vcpkg
- [This](https://learn.microsoft.com/en-gb/vcpkg/get_started/get-started?pivots=shell-bash) tutorial was used to set up [Vcpkg](https://vcpkg.io/en/package/libxmlpp)
- I am still getting issues. [Could not find package configuration file](https://stackoverflow.com/questions/65045150/how-to-fix-could-not-find-a-package-configuration-file-error-in-cmake) is one of them and it may be an issue with the Libxmlpp library?

### PkgConfig and CMake
- It turns out that Libxml++ does not have a CMake configuration by design, as it is instead handled by PkgConfig which requires a special configuration in the CMakeLists.txt file, as I discovered [here](https://stackoverflow.com/questions/63749077/c-cmake-cant-find-libxml)
- [This](https://stackoverflow.com/questions/29191855/what-is-the-proper-way-to-use-pkg-config-from-cmake) guide shows how to properly configure CMake with PkgConfig. Now that this is done, Libxml++ successfully links and builds in the project. An example from the libxml++ Github was tested to confirm this.
- Next, the documentation needs to be built, and the Clang language server needs to be configured so that it can see the libraries included by CMake as currently it shows errors.


