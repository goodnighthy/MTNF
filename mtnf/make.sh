make_func() {
    cd nfs
    make clean
    make install

    cd ../libs
    make clean
    make install

    cd ../framework
    make
    ./go.sh 0,1 1
}

make_func