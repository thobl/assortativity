
read <- function(name) {
    #' read csv file in output data folder
    read.csv(paste0("../output_data/", name, ".csv"))
}

read_real_stats <- function() {
    stats <- read("stats") %>%
        filter(type == "undirected", !startsWith(graph, "cl_"), !startsWith(graph, "girg_")) %>%
        select(graph, n, m, pearson, spearman, starts_with("kendall_"))
    konect <- read.csv("../input_data/power-law-networks/undirected.csv") %>%
        select(graph = konect_name, starts_with("ple_"), konect_display_name = graph) %>%
        mutate(konect_display_name = gsub(" \\(.*\\)$", "", konect_display_name))

    left_join(stats, konect, by = "graph") %>%
        filter(ple_type %in% c("strong", "weak"))
}

read_gen_stats <- function(model) {
    stats <- read("stats") %>%
        filter(type == "undirected", startsWith(graph, paste0(model, "_"))) %>%
        select(graph, n, m, pearson, spearman, starts_with("kendall_"))
    gen <- read.csv(paste0("../output_data/", model, "_attributes.csv")) %>%
        rename(gen_n = n)

    left_join(stats, gen, by = "graph")
}
