mkdir build-windows-%PROCESSOR_ARCHITECTURE%
cd build-windows-%PROCESSOR_ARCHITECTURE%
cmake ..
cmake --build .
cd ..
PAUSE