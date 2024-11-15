#!/bin/bash

# publish to pypi if argument is RELEASE
if [ "$1" == "RELEASE" ]; then
    python3 -m twine upload --verbose --repository pypi wheelhouse/*
# publish to testpypi if argument is TEST
elif [ "$1" == "TEST" ]; then
    python3 -m twine upload --verbose --repository testpypi wheelhouse/*
# error message if argument is not RELEASE or TEST
else
    echo "Please provide either RELEASE or TEST as an argument"
fi
