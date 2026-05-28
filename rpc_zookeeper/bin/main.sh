#!/bin/bash

echo "now in:`pwd`"

exe_file="$1"
config_file="$2"
if [ -f $exe_file ]; then
    echo "$exe_file exist"
else
    echo "$exe_file not exist, please check!"
    return 1
fi

if [ -f $config_file ]; then
    echo "$config_file exist"
else
    echo "$config_file not exist, please check!"
    return 1
fi

.\/$exe_file -i $config_file
echo "server start"