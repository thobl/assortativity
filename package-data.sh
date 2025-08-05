#!/bin/bash

mkdir -p data-packs

FILE="output_data.zip"
if [ ! -f "data-packs/$FILE" ]; then
    echo "crating $FILE"
    zip -q -r "data-packs/$FILE" \
        "output_data/" \
        -x "output_data/.gitignore" "output_data/merge-csv.sh"
else
    echo "skipping $FILE (already exists)"
fi


FILE="real_world_networks.zip"
if [ ! -f "data-packs/$FILE" ]; then
    echo "crating $FILE"
    zip -q -r "data-packs/$FILE" \
        "input_data/graphs/" \
        -x '*_seed=*'
else
    echo "skipping $FILE (already exists)"
fi


FILE="generated_networks.zip"
if [ ! -f "data-packs/$FILE" ]; then
    echo "crating $FILE"
    zip -q -r "data-packs/$FILE" \
        "input_data/graphs/" \
        -i "*_seed=*"
else
    echo "skipping $FILE (already exists)"
fi
