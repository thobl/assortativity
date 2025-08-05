#!/bin/bash 

for file in *.ipe; do
    ipetoipe -pdf "$file"
done

pdftk vis_*.pdf cat output out.pdf
