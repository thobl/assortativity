source("helper/load_helpers.R")
library(scales)
library(tidyr)
library(purrr, warn.conflicts = FALSE)

prep_gen_data <- function(model) {
    read_gen_stats(model) %>%
        filter(seed > 50) %>% # this filters out girgs with T > 0
        mutate(model = toupper(model)) %>%
        select(graph, model, n = gen_n, pearson, spearman, kendall = kendall_tau_simple, ple, sigma, seed) %>%
        pivot_longer(c(pearson, spearman, kendall), values_to = "assortativity", names_to = "type") %>%
        mutate(type = factor(type, levels = c("pearson", "spearman", "kendall")))
}

tbl <- c("girg", "cl") %>% map_dfr(prep_gen_data)
# head(tbl)


# #E69F00 #56B4E9 #009E73 #F0E442 #0072B2 #D55E00 #CC79A7 #999999
colors <- palette.colors(palette = "Okabe-Ito")[-1]

common <- list(
    stat_summary(fun.y = mean, geom = "line", size = 0.65),
    stat_summary(fun.y = mean, geom = "point", size = 1.5),
    scale_color_manual(values = colors[-4])
)


######################################################################
## scaling with n
######################################################################

n_breaks <- tbl %>%
    arrange(n) %>%
    pull(n) %>%
    unique()

common_size_scaling <- list(
    scale_x_log10(breaks = n_breaks[-1], label = label_number(scale_cut = cut_si(""))),
    labs(x = "n", color = "σ")
)

tbl_size_scaling <- tbl %>%
    filter(sigma %in% c(0.2, 0.6, 1, 1.4, 1.8))

ggplot(
    tbl_size_scaling %>% filter(type == "spearman"),
    aes(x = n, y = assortativity, color = as.factor(sigma), group = as.factor(sigma))
) +
    facet_grid(rows = vars(ple), cols = vars(model)) +
    common +
    common_size_scaling
ggsave("output/assortativity_scale_n.pdf", width = 5, height = 7.5)
ggsave("output/assortativity_scale_n.eps", width = 5, height = 7.5)

ggplot(
    tbl_size_scaling %>% filter(ple == 2.6),
    aes(x = n, y = assortativity, color = as.factor(sigma), group = as.factor(sigma))
) +
    facet_grid(cols = vars(model), rows = vars(type)) +
    common +
    common_size_scaling
ggsave("output/assortativity_scale_n_ple=2.6_with_kendall.pdf", width = 5, height = 3.75)
ggsave("output/assortativity_scale_n_ple=2.6_with_kendall.eps", width = 5, height = 3.75)

ggplot(
    tbl_size_scaling %>% filter(ple == 2.6, type != "kendall"),
    aes(x = n, y = assortativity, color = as.factor(sigma), group = as.factor(sigma))
) +
    facet_grid(cols = vars(model), rows = vars(type)) +
    common +
    common_size_scaling
ggsave("output/assortativity_scale_n_ple=2.6.pdf", width = 5, height = 2.725)
ggsave("output/assortativity_scale_n_ple=2.6.eps", width = 5, height = 2.725)


######################################################################
## scaling with σ
######################################################################

sigma_breaks <- tbl %>%
    arrange(sigma) %>%
    pull(sigma) %>%
    unique()
sigma_breaks <- sigma_breaks[seq(1, length(sigma_breaks), 2)]

ggplot(
    tbl %>% filter(n == 200000, type %in% c("spearman", "pearson")),
    aes(x = sigma, y = assortativity, color = as.factor(ple), group = as.factor(ple))
) +
    facet_grid(rows = vars(type), cols = vars(model)) +
    scale_x_continuous(breaks = sigma_breaks) +
    common +
    labs(x = "σ", color = "τ")
ggsave("output/assortativity_scale_sigma.pdf", width = 5, height = 3)
ggsave("output/assortativity_scale_sigma.eps", width = 5, height = 3)
