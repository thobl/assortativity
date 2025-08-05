#!/bin/bash

mkdir -p output
mkdir -p output-individual-graphs

HEADBEG="\e[36m\e[1m"
HEADEND="\e[0m"

scripts=(
    ## "assortativity-gen.R" 
    ## "assortativity-real.R"
    "degree_distr_all.R" ## (many plots)
    # "degree_distr_selected.R"
    # "real-table.R"
)

for script in "${scripts[@]}"; do
    echo ""
    echo -e "${HEADBEG}creating plots:${HEADEND} $script"

    if [ ! -f "renv/activate.R" ]; then
        Rscript --no-init-file -e "options(warn=-1); source(\"$script\")"
    else
        Rscript -e "options(warn=-1); source(\"$script\")"
    fi
done

echo ""
