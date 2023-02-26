cd ..
mkdir build
cd build
cmake ..
make
cd ../src
python3 test.py
cd ..
rm -r build/