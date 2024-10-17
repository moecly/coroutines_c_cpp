#!/bin/bash

case "$1" in
    cl)
        make -j18 clean
        ;;
    mk)
        make -j18 all
        ;;
    re)
        make -j18 clean
        make -j18 all
        ;;
    "")
        make -j18 all
        ;;
    *)
        ;;
esac