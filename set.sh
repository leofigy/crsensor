cd ./clib
make clean
make
cd ..
echo "copying library to src dir"
cp ./clib/binsensor.o src/