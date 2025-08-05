#!/usr/bin/env python3
import os
import glob
import run
import multiprocessing

import konect

run.use_cores(3)

os.makedirs("input_data/graphs", exist_ok=True)

######################################################################
run.section("[1/3] Download and generate graphs")


######################################################################
run.group("konect")

run.add(
    "download&unpack",
    "cd input_data/konect-download/ && ./downloader.sh [[graph]]",
    {"graph": konect.graph_names_undirected},
    creates_file="input_data/graphs/[[graph]]",
)


######################################################################
run.group("generate")

common_params = {
    "n": [12500, 25000, 50000, 100000, 200000],
    "deg": 15,
    "ple": [2.2, 2.4, 2.6, 2.8],
    "sigma": [0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8],
    "seed": [64, 321, 356, 489, 836],
}

run.add(
    "girg",
    "cpp/release/girg -n [[n]] --deg [[deg]] --dim [[dim]] --ple [[ple]] -T [[T]] --sigma [[sigma]] --seed [[seed]] [[file]]",
    common_params
    | {
        "dim": 2,
        "T": 0,
        "name": "girg_n=[[n]]_deg=[[deg]]_dim=[[dim]]_ple=[[ple]]_T=[[T]]_sigma=[[sigma]]_seed=[[seed]]",
        "file": "input_data/graphs/[[name]]",
    },
    stdout_file="output_data/girg_attributes/[[name]].csv",
)

run.add(
    "cl",
    "cpp/release/chung_lu -n [[n]] --deg [[deg]] --ple [[ple]] --sigma [[sigma]] --seed [[seed]] [[file]]",
    common_params
    | {
        "name": "cl_n=[[n]]_deg=[[deg]]_ple=[[ple]]_sigma=[[sigma]]_seed=[[seed]]",
        "file": "input_data/graphs/[[name]]",
    },
    stdout_file="output_data/cl_attributes/[[name]].csv",
)


run.add(
    "girg-warm",
    "cpp/release/girg -n [[n]] --deg [[deg]] --dim [[dim]] --ple [[ple]] -T [[T]] --sigma [[sigma]] --seed [[seed]] [[file]]",
    {
        "n": 200000,
        "deg": 15,
        "ple": [2.2, 2.4, 2.6, 2.8],
        "sigma": [0.2, 0.4, 0.6, 0.8, 1.0, 1.2, 1.4, 1.6, 1.8],
        "seed": 17,
        "dim": 2,
        "T": [0.5, 0.7, 0.9],
        "name": "girg_n=[[n]]_deg=[[deg]]_dim=[[dim]]_ple=[[ple]]_T=[[T]]_sigma=[[sigma]]_seed=[[seed]]",
        "file": "input_data/graphs/[[name]]",
    },
    stdout_file="output_data/girg_attributes/[[name]].csv",
)

run.add(
    "girg-vis",
    "cpp/release/girg -n [[n]] --deg [[deg]] --dim [[dim]] --ple [[ple]] -T [[T]] --sigma [[sigma]] --seed [[seed]] --ipe [[file]]",
    {
        "n": 1000,
        "deg": 10,
        "dim": 2,
        "ple": 2.8,
        "T": 0,
        "sigma": [x / 10 for x in range(1, 19)],
        "seed": 64,
        "name": "vis_n=[[n]]_deg=[[deg]]_dim=[[dim]]_ple=[[ple]]_T=[[T]]_sigma=[[sigma]]_seed=[[seed]]",
        "file": "vis/[[name]]",
    },
    stdout_file="output_data/vis/[[name]].csv",
)

######################################################################
run.group("rm_generated")

# names of all output stats
output_names = [
    os.path.basename(os.path.dirname(dir)) for dir in glob.glob("output_data/*/")
]

# remove all graphs/results with certain prefix
prefixes = ["girg", "cl"]
run.add(
    "rm_[[prefix]]",
    "rm -f input_data/graphs/[[prefix]]_n*",
    {
        "prefix": prefixes,
    },
)

run.add(
    "rm_[[prefix]]",
    "rm -f output_data/[[output]]/[[prefix]]_n*",
    {"output": output_names, "prefix": prefixes},
)

run.run()


######################################################################
run.section("[2/3] Compute stuff")

graph_names = [os.path.basename(f) for f in glob.glob("input_data/graphs/*")]


######################################################################
run.group("stats")

run.add(
    "basic_stats",
    "cpp/release/stats input_data/graphs/[[graph]] --no-header",
    {"graph": graph_names},
    stdout_file="output_data/stats/[[graph]].csv",
    header_command="cpp/release/stats --only-header",
)

run.add(
    "degree_distr",
    "cpp/release/degree_distribution [[edge]] [[bucket_arg]] input_data/graphs/[[graph]] --no-header",
    {
        "graph": graph_names,
        "edge": ["--edge_endpoints", ""],
        "nr_buckets": 21,
        "bucket": ["", 0, 5, 10, 15, 20],
        "bucket_arg": lambda args: (
            "--nr_buckets [[nr_buckets]] --bucket [[bucket]]"
            if args["edge"] != "" and args["bucket"] != ""
            else ""
        ),
    },
    # do not use buckets without edge
    combinations_filter=lambda args: (args["edge"] != "" or args["bucket"] == ""),
    stdout_file="output_data/degree_distr/[[graph]].csv",
    header_command="cpp/release/degree_distribution --only-header",
)

run.add(
    "joint_histogram",
    "cpp/release/joint_histogram input_data/graphs/[[graph]] --nr_buckets=21 --no-header",
    {"graph": graph_names},
    stdout_file="output_data/joint_histogram/[[graph]].csv",
    header_command="cpp/release/joint_histogram --only-header",
)

run.run()


######################################################################
run.section("[3/3] Postprocessing of the results")

output_names = [  # might have changed since previous run
    os.path.basename(os.path.dirname(dir)) for dir in glob.glob("output_data/*/")
]


######################################################################
run.group("post")

run.add(
    "merge_csv",
    "output_data/merge-csv.sh output_data/[[output]]",
    {"output": output_names},
    creates_file="output_data/[[output]].csv",
)

run.add(
    "merge_csv_force",
    "output_data/merge-csv.sh output_data/[[output]]",
    {"output": output_names},
)

run.run()
