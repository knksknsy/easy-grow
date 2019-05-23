#!/bin/bash

if  hash git && hash gcc && hash pip && [ -d "$IDF_PATH" ]; then
    echo 'Needed software is installed.'
else
    exit 1
fi
    

