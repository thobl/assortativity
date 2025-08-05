#!/usr/bin/env bash

wget http://konect.cc/files/download.tsv.$1.tar.bz2
bunzip2 -c download.tsv.$1.tar.bz2 | tar xvf -
rm download.tsv.$1.tar.bz2
mv $1/out.* ../graphs/$1
