#!/bin/bash
build_file="`pwd`/build"

if  [ -d "$build_file" ];then
    echo "build dir exist"
else
    mkdir $build_file
    echo "build dir create"
fi

gnome-terminal --title "server" --working-directory $build_file -- bash -c \
    'make clean; 
    make;
    bin_file="`pwd`/../bin"

    func(){
        exe_file="$1"
        config_file="$2"
        if [ -d $bin_file  ]; then
        cd $bin_file && ./main.sh $exe_file $config_file
        echo $bin_file exist
    else
        echo $bin_file not exist, please check!
        return 1
    fi
    }

    func userserver a.config
    exec bash;'

