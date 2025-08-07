[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.16746826.svg)](https://doi.org/10.5281/zenodo.16746826)

# Assortativity in Geometric and Scale-Free Networks #

This repository contains the code to run the experiments form the paper

> Marc Kaufmann, Ulysse Schaller, Thomas Bläsius, Johannes Lengler:
> *Assortativity in Geometric and Scale-Free Networks.*
> [arxiv.org/abs/2508.04608](https://arxiv.org/abs/2508.04608)


Running the experiments consists of the following steps:
  1. install dependencies
  2. build the c++ code
  3. download real-world networks
  4. generate networks
  5. compute network statistics
  6. create plots

Once the dependencies are set up (step 1), steps 2-6 can all be done with
just a few commands:

```terminal
./build.sh
./experiments.py konect generate stats post
cd R && ./create-plots.sh
```

Some of the steps can be skipped, e.g., if you just want to explore
the data (step 6), you can skip steps 2-5 by downloading the data from
[Zenodo](https://zenodo.org/record/16745980).  Moreover, for some
steps there are different options (the big one being whether you use
docker for the dependencies).  We outline these alternatives for the
individual steps below.

## 1. Dependencies ##

You can use docker or run the code natively on your machine.

### Docker ###

From the root folder, just run:

```terminal
sudo docker compose run experiments
```

This will put you inside a docker container that has all the
dependencies for running the experiments (steps 2-5).  After step 5,
you can type `exit` to exit the container.  To evaluate the
experiments (step 6), enter the evaluation container:

```terminal
sudo docker compose run eval
```

When first running these commands, the docker containers are built
(which may take a few minutes).


### Native ###

You need a basic development environment (`g++`, `cmake`, `git`,
`python`).  Additionally, you need the python module
[run](https://github.com/thobl/run), which can be installed using
`pip install run-experiments==1.0.8`.

For creating the plots, you additionally need `R` with the package
`renv` (which takes care of installing other `R` packages in a local
environment).  To build additional dependencies, you need `cmake` and
`libfreetype6-dev`.  Additionally, you need the LModern font
(`fonts-lmodern`).

To use `renv` to install the required `R` packages in the correct
version, run the following (which may take some time):

```terminal
cd R/
Rscript -e "renv::restore()"
```

**Note:** For the font to work, you need to make sure that `R` can
find the file `lmroman10-regular.otf`.  If it does not, maybe have a
look at [`docker/eval/Dockerfile`](docker/eval/Dockerfile) (or just
use docker).


## 2. Building the C++ Code ##

> In case you use docker, run the commands in this section from inside
> the experiments container (`sudo docker compose run experiments`).

Simply run:

```terminal
./build.sh
```

This will download the [girgs](https://github.com/chistopher/girgs)
library for generating GIRGs (using `git submodule`) and then build
the c++ code in `cpp/release/`.

## 3-5. Download/Generate Networks and Compute Stats ##

> In case you use docker, run the commands in this section from inside
> the experiments container (`sudo docker compose run experiments`).

Steps 3-5 can be done with just a single command:

```terminal
./experiments.py konect generate stats post
```

These steps can also be done individually (with different options) and
they can potentially be skipped.  Thus, we additionally discuss the
steps individually in the following.

### 3. Download Real-World Networks ###

Running `./experiments.py konect` will download and unzip the
real-world networks form [Konect](http://konect.cc/).

You can alternatively download the networks from
[Zenodo](https://zenodo.org/record/16745980) (e.g., if Konect is not
available) and place them in the folder `input_data/graphs/`.  Using
the command line, this can be done by running the following commands
from the root of this repository:

```terminal
wget https://zenodo.org/record/16745980/files/real_world_networks.zip
unzip real_world_networks.zip
```

### 4. Generate Networks ###

Running `./experiments.py generate` will generate networks.  If you
don't want to generate them yourselves, you can instead download them
from [Zenodo](https://zenodo.org/record/16745980) and place them into
the folder `input_data/graphs/`.

```terminal
wget https://zenodo.org/record/16745980/files/generated_networks.zip
unzip generated_networks.zip
```
However, it is likely faster to generate them than to download them.

### 5. Compute Network Statistics ###

Running `./experiments.py stats post` will generate the network stats
in `output_data/`.  If yo don't want to compute the statistics
yourselves, you can instead download them from
[Zenodo](https://zenodo.org/record/16745980) and place them into the
folder `output_data/`.

```terminal
wget https://zenodo.org/record/16745980/files/output_data.zip
unzip output_data.zip
```

**Note:** If you choose to download the data created in step 5 from
Zenodo, you can completely skip steps 3 and 4 (as you don't need the
graph data).  You then also don't need any dependencies, except for
step 6.

## 6. Create Plots ##

> In case you use docker, run the commands in this section from inside
> the **evaluation** container (`sudo docker compose run eval`).

From the `R/` folder, run `./create-plots.sh` to create most plots
appearing in the paper.  If want to create many (> 1000) additional
plots (which takes a few minutes), you can run
`./create-many-additional-plots.sh`.

The resulting plots are written to `R/output/` and
`R/output-individual-graphs/`.

## Miscellaneous ##

### How Long Does It Take? ###

Running the experiments should not take too long.  On my laptop,
downloading the real-world networks from Konect took about 3min,
generating networks took 11min, and computing the stats 40min.

The experiments are parallelized to run three tasks at a time.  If you
have a beefy machine (i.e., you have enough main memory), you can
increase the number of cores by editing this line in `experiments.py`:

```python
run.use_cores(3)
```

You can of course also use this to decrease the number of cores, in
case your memory runs out when processing multiple larger networks.


### Visualization ###

Running the experiments also creates a folder `vis/` that contains
visualizations of TGIRGs with different values for σ. To actually
create the visualization, you have to run `./aggregate.sh` from the
`vis/` folder.  For this to work, you need
[Ipe](https://ipe.otfried.org/) and `pdftk` installed.
