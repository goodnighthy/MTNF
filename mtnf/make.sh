make_func() {
    cd nfs
    make clean
    make install

    cd ../libs
    make clean
    make install

    cd ../framework
    make
    ./go.sh 0,1,2,3,4,5,6,7,8 0xff
}

make_func