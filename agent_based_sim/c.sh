mkdir -p build; 

cd build; 
#cmake --verbose ../src;
cmake -DCMAKE_BUILD_TYPE=Debug ../src;
make -j8 libsalt salt-standalone-mpduar-event salt-standalone-event
