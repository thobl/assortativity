source("helper/load_helpers.R")
library(ggrepel)
library(ggpubr)

tbl <- read_real_stats() %>%
    mutate(kendall = kendall_tau_simple, kendall_b = kendall_tau_b, kendall_a = kendall_tau_a) %>%
    mutate(type = if_else(ple_hill < 7 / 3, "τ < 7/3",
        if_else(ple_hill <= 3, "7/3 ≤ τ ≤ 3", "3 < τ")
        ),
        type = factor(type, levels = c("τ < 7/3", "7/3 ≤ τ ≤ 3", "3 < τ"))
        )
# tbl

# #E69F00 #56B4E9 #009E73 #F0E442 #0072B2 #D55E00 #CC79A7 #999999
colors <- palette.colors(palette = "Okabe-Ito")[-1]

plot_compare <- function(xx, yy) {
    ggplot(tbl, aes(x = {{ xx }}, y = {{ yy }}, label = konect_display_name, fill = type)) +
        geom_abline(slope = 1, intercept = 0, color = "gray") +
        geom_label_repel(
            force = 40,
            min.segment.length = 0, segment.size = 0.3,
            size = 1.7,
            label.padding = 0.1, label.size = 0.1, label.r = 0.1,
            box.padding = 0.1
        ) +
        geom_point(aes(color = type), size = 0.7) +
        lims(x = c(-0.6, 0.6), y = c(-0.6, 0.6)) +
        theme(aspect.ratio = 1) +
        labs(color = "power-law\nexponent τ") +
        guides(
            fill = "none",
            color = guide_legend(override.aes = list(size = 2))
        ) +
        scale_fill_manual(values = colors) +
        scale_color_manual(values = colors)
}

p <- ggarrange(
    plot_compare(pearson, spearman) + theme(legend.title = element_blank()),
    ggplot() + theme_void(),
    plot_compare(kendall, spearman) +
        theme(
            axis.title.y = element_blank(),
            axis.text.y = element_blank(),
            axis.ticks.y = element_blank(),
            plot.background = element_rect(fill = "transparent", color = NA)
        ),
    nrow = 1, widths = c(1, -0.17, 1),
    common.legend = TRUE,
    legend = "right",
    align = "hv"
)


ggsave("output/assortativity_real.pdf", p, width = 5, height = 2.2)
ggsave("output/assortativity_real.eps", width = 5, height = 2.2)


p <- ggarrange(
    plot_compare(kendall_a, kendall),
    plot_compare(kendall_b, kendall) +
        theme(
            axis.title.y = element_blank(),
            axis.text.y = element_blank(),
            axis.ticks.y = element_blank()
        ),
    nrow = 1
)
ggsave("output/assortativity_real_kendall_variants.pdf", p, width = 15, height = 7)
