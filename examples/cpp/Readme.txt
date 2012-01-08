Each subfolder contains a simple example C++ application that does a certain 
task. See the Readme.txt in each subfolder for a description and further 
information to each example.

All examples consist of a main.cpp and can be built with cmake when both Qt and 
Marble development packages are installed. A compiled version of Marble can be 
used as well. To compile and run an example with Qt and Marble installed in the 
system, follow this example:

cd hello-marble
cmake .
make
./hello-marble

You can also use a local Marble version like this:

cd hello-marble
cmake -DCMAKE_MODULE_PATH=~/marble/export/share/marble/cmake \
      -DCMAKE_INCLUDE_PATH=~/marble/export/include \
      -DCMAKE_LIBRARY_PATH=~/marble/export/lib \
      .
make
./hello-world

In the second example Marble is assumed to be installed to ~/marble/export.
