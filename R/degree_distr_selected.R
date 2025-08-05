source("degree_distr.R")

read_multi <- function(graphs, fun) {
    graphs %>%
        lapply(fun) %>%
        bind_rows()
}

fix_facet_order <- function(tbl, graphs) {
    tbl %>%
        mutate(graph_display = factor(graph_display,
            levels = lapply(graphs, graph_display_name)
        ))
}

######################################################################
## initial plot for introducing the different plot types (CL)

graphs <- c(
    "cl_n=200000_deg=15_ple=2.8_sigma=0.2_seed=64",
    "cl_n=200000_deg=15_ple=2.8_sigma=1.0_seed=64",
    "cl_n=200000_deg=15_ple=2.8_sigma=1.6_seed=64"
)

tbl_deg_distr <- graphs %>%
    read_multi(read_deg_distr) %>%
    fix_facet_order(graphs)
buckets <- deg_buckets(tbl_deg_distr) %>% fix_facet_order(graphs)
tbl_joint <- graphs %>%
    read_multi(read_joint_distr) %>%
    fix_facet_order(graphs)

plot_joint(tbl_joint) + bucket_lines(buckets) + labs(caption = NULL) +
    theme(
        legend.position = "bottom", legend.title = element_text(vjust = 0.9),
        legend.key.height = unit(0.3, "cm"),
        legend.key.width = unit(1, "cm"),
        legend.margin = margin(-3, 0, 0, 0)
    )
ggsave("output/deg_distr_cl_joint_selected.eps", width = 5, height = 2.25)
ggsave("output/deg_distr_cl_joint_selected.pdf", width = 5, height = 2.25)

plot_deg_distr(tbl_deg_distr) + labs(caption = NULL) +
    theme(
        legend.position = "bottom",
        legend.key.width = unit(0.5, "cm"),
        legend.key.height = unit(0.01, "cm"),
        legend.margin = margin(-3, 0, 0, 0)
    ) +
    guides(color = guide_legend(ncol = 7))
ggsave("output/deg_distr_cl_selected.eps", width = 5, height = 2)
ggsave("output/deg_distr_cl_selected.pdf", width = 5, height = 2)

plot_cond_rel(tbl_joint) + labs(caption = NULL) +
    theme(
        legend.position = "bottom", legend.title = element_text(vjust = 0.9),
        legend.key.height = unit(0.3, "cm"),
        legend.key.width = unit(0.5, "cm"),
        legend.margin = margin(-3, 0, 0, 0)
    )
ggsave("output/deg_distr_cl_change_selected.eps", width = 5, height = 2.35)
ggsave("output/deg_distr_cl_change_selected.pdf", width = 5, height = 2.35)


######################################################################
## plot for discussing generated graphs

graphs <- c(
    "cl_n=200000_deg=15_ple=2.8_sigma=0.2_seed=64",
    "cl_n=200000_deg=15_ple=2.8_sigma=1.0_seed=64",
    "cl_n=200000_deg=15_ple=2.8_sigma=1.6_seed=64",
    "girg_n=200000_deg=15_dim=2_ple=2.8_T=0.7_sigma=0.2_seed=17",
    "girg_n=200000_deg=15_dim=2_ple=2.8_T=0.7_sigma=1.0_seed=17",
    "girg_n=200000_deg=15_dim=2_ple=2.8_T=0.7_sigma=1.6_seed=17",
    "girg_n=200000_deg=15_dim=2_ple=2.8_T=0_sigma=0.2_seed=64",
    "girg_n=200000_deg=15_dim=2_ple=2.8_T=0_sigma=1.0_seed=64",
    "girg_n=200000_deg=15_dim=2_ple=2.8_T=0_sigma=1.6_seed=64"
)

tbl_joint <- graphs %>%
    read_multi(read_joint_distr) %>%
    fix_facet_order(graphs)

plot_cond_rel(tbl_joint) + labs(caption = NULL) +
    theme(
        legend.position = "bottom", legend.title = element_text(vjust = 0.9),
        legend.key.height = unit(0.3, "cm"),
        legend.key.width = unit(0.5, "cm"),
        legend.margin = margin(-3, 0, 0, 0)
    )

ggsave("output/deg_distr_change_generated.eps", width = 5, height = 5.8)
ggsave("output/deg_distr_change_generated.pdf", width = 5, height = 5.8)


######################################################################
## plot for discussing real-world graphs

graphs <- c(
    "petster-friendships-cat",
    "petster-friendships-dog",
    "petster-carnivore",
    "com-youtube",
    "loc-brightkite_edges",
    "loc-gowalla_edges",
    "as-caida20071105",
    "as20000102",
    "as-skitter"
)
tbl_deg_distr <- graphs %>%
    read_multi(read_deg_distr) %>%
    fix_facet_order(graphs)
buckets <- deg_buckets(tbl_deg_distr) %>% fix_facet_order(graphs)
tbl_joint <- graphs %>%
    read_multi(read_joint_distr) %>%
    fix_facet_order(graphs)

plot_cond_rel(tbl_joint) + labs(caption = NULL) +
    theme(
        legend.position = "bottom", legend.title = element_text(vjust = 0.9),
        legend.key.height = unit(0.3, "cm"),
        legend.key.width = unit(0.5, "cm"),
        legend.margin = margin(-3, 0, 0, 0)
    ) +
    facet_wrap(vars(graph_display), ncol = 3, scales = "free")

ggsave("output/deg_distr_change_real.eps", width = 5, height = 5.8)
ggsave("output/deg_distr_change_real.pdf", width = 5, height = 5.8)

plot_deg_distr(tbl_deg_distr) + labs(caption = NULL) +
    theme(
        legend.position = "bottom",
        legend.key.width = unit(0.5, "cm"),
        legend.key.height = unit(0.01, "cm"),
        legend.margin = margin(-3, 0, 0, 0)
    ) +
    guides(color = guide_legend(ncol = 7)) +
    facet_wrap(vars(graph_display), ncol = 3, scales = "free_x")

ggsave("output/deg_distr_line_real.eps", width = 5, height = 5)
ggsave("output/deg_distr_line_real.pdf", width = 5, height = 5)


######################################################################
## generated graphs comparison to real-world

graphs <- c(
    "cl_n=200000_deg=15_ple=2.2_sigma=1.2_seed=64",
    "cl_n=200000_deg=15_ple=2.2_sigma=0.8_seed=64",
    "girg_n=200000_deg=15_dim=2_ple=2.4_T=0_sigma=0.8_seed=64"
)

tbl_joint <- graphs %>%
    read_multi(read_joint_distr) %>%
    fix_facet_order(graphs)

plot_cond_rel(tbl_joint) + labs(caption = NULL) +
    theme(
        legend.position = "bottom", legend.title = element_text(vjust = 0.9),
        legend.key.height = unit(0.3, "cm"),
        legend.key.width = unit(0.5, "cm"),
        legend.margin = margin(-3, 0, 0, 0)
    ) +
    facet_wrap(vars(graph_display), ncol = 3, scales = "free")

ggsave("output/deg_distr_change_comparison.eps", width = 5, height = 2.35)
ggsave("output/deg_distr_change_comparison.pdf", width = 5, height = 2.35)

######################################################################
## generated graphs comparison to real-world:  social

graphs <- c(
    "loc-gowalla_edges",
    "com-youtube",
    "petster-friendships-cat",
    "girg_n=200000_deg=15_dim=2_ple=2.8_T=0.5_sigma=1.6_seed=17",
    "girg_n=200000_deg=15_dim=2_ple=2.2_T=0.5_sigma=1.0_seed=17",
    "girg_n=200000_deg=15_dim=2_ple=2.2_T=0.5_sigma=0.8_seed=17"
)

tbl_joint <- graphs %>%
    read_multi(read_joint_distr) %>%
    fix_facet_order(graphs)

plot_cond_rel(tbl_joint) + labs(caption = NULL) +
    theme(
        legend.position = "bottom", legend.title = element_text(vjust = 0.9),
        legend.key.height = unit(0.3, "cm"),
        legend.key.width = unit(0.5, "cm"),
        legend.margin = margin(-3, 0, 0, 0)
    ) +
    facet_wrap(vars(graph_display), ncol = 3, scales = "free")

ggsave("output/deg_distr_change_comparison_social.eps", width = 5, height = 3.9)
ggsave("output/deg_distr_change_comparison_social.pdf", width = 5, height = 3.9)

######################################################################
## generated graphs with varying power-law exponent

graphs <- c(
    "cl_n=200000_deg=15_ple=2.2_sigma=1.0_seed=64",
    "cl_n=200000_deg=15_ple=2.4_sigma=1.0_seed=64",
    "cl_n=200000_deg=15_ple=2.6_sigma=1.0_seed=64",
    "cl_n=200000_deg=15_ple=2.8_sigma=1.0_seed=64"
)

tbl_joint <- graphs %>%
    read_multi(read_joint_distr) %>%
    fix_facet_order(graphs)

plot_cond_rel(tbl_joint) + labs(caption = NULL) +
    theme(
        legend.position = "bottom", legend.title = element_text(vjust = 0.9),
        legend.key.height = unit(0.3, "cm"),
        legend.key.width = unit(0.5, "cm"),
        legend.margin = margin(-3, 0, 0, 0)
    ) +
    facet_wrap(vars(graph_display), ncol = 4, scales = "free")

ggsave("output/deg_distr_change_neutral_assort.eps", width = 5, height = 1.8)
ggsave("output/deg_distr_change_neutral_assort.pdf", width = 5, height = 1.8)

######################################################################
## comparison for the intro

graphs <- c(
    "loc-gowalla_edges",
    "as-caida20071105",
    "cl_n=200000_deg=15_ple=2.4_sigma=1.0_seed=64",
    "com-youtube"
)

tbl_joint <- graphs %>%
    read_multi(read_joint_distr) %>%
    fix_facet_order(graphs)

plot_cond_rel(tbl_joint) + labs(caption = NULL) +
    facet_wrap(vars(graph_display), ncol = 2, scales = "free")

ggsave("output/deg_distr_change_intro.eps", width = 5, height = 4)
ggsave("output/deg_distr_change_intro.pdf", width = 5, height = 4)
