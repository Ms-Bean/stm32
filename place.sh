#!/bin/bash

for f in *
do
    if [ -f $f ] && [ $f != `basename $0` ]
    then
        scp $f raspberrypi:~/stm/STM32-Template/HD44780/$f
    fi
done
