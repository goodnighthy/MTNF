make_func() {
    cd nfs
    make clean
    make install

    cd ../libs
    make clean
    make install

    # cd ../framework
    cd ../framework_software
    make
    ./go.sh 0,2,4,6,8,10 3
}

make_func