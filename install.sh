pip install -e frontend

if [ ! -e build/ ]; then
    mkdir -p build/
    cmake -S . -B build/
fi
make -C build
sudo make install -C build
