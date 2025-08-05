#!/bin/bash 

HL="\e[32m\e[1m"
CODE="\e[36m"
END="\e[0m"

echo ""
echo -e "Welcome to the ${HL}massortativity experiments${END} docker container."
echo ""
echo -e "You first need to build the cpp code:"
echo -e "    ${CODE}./build.sh${END}"
echo ""
echo -e "To then run all, run:"
echo -e "    ${CODE}./experiments.py konect generate stats post${END}"
echo ""
echo -e "The ${CODE}konect${END} part downloads real-wold networks from Konect. Due to ${CODE}generate${END}, it will generate networks. The ${CODE}stats${END} computes the network statistics and ${CODE}post${END} some post-processing."
echo -e "These parts can also be run individually. Also see README.md for more info."
echo ""
