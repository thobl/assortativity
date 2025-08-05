source("helper/load_helpers.R")
library(ggnewscale)
library(scales)


################################################################################
################################################################################
## reading and preparing data
################################################################################
################################################################################

## ----------------------------------------------------------------------------
## helper
## ----------------------------------------------------------------------------

read_with_graph_name <- function(dir, graph) {
    #' read csv file for a single graph and add column with graph name
    display_name <- graph_display_name(graph)
    read(paste0(dir, "/", graph)) %>%
        mutate(
            graph = graph,
            graph_display = display_name
        ) %>%
        relocate(graph, .before = 1)
}



graph_display_name <- function(graph_name) {
    if (startsWith(graph_name, "girg_")) {
        attr <- read(paste0("girg_attributes/", graph_name))
        return(paste0("GIRG (τ = ", attr$ple, ", σ = ", attr$sigma, ", T = ", attr$T, ")"))
    } else if (startsWith(graph_name, "cl_")) {
        attr <- read(paste0("cl_attributes/", graph_name))
        return(paste0("CL (τ = ", attr$ple, ", σ = ", attr$sigma, ")"))
    }
    read_real_stats() %>%
        filter(graph == graph_name) %>%
        pull(konect_display_name)
}



## ----------------------------------------------------------------------------
## regular degree distribution
## ----------------------------------------------------------------------------

read_deg_distr <- function(graph) {
    #' degree distribution, i.e., pairs of (degree, frequency)
    read_with_graph_name("degree_distr", graph) %>%
        mutate(type = deg_distr_type(
            edge_endpoints, nr_buckets,
            bucket, neighbor_deg_lb,
            neighbor_deg_ub
        )) %>%
        group_by(type) %>%
        arrange(degree, .by_group = TRUE) %>%
        mutate(
            n = sum(frequency),
            fraction_greater_equal = (n - cumsum(frequency) + frequency) / n
        ) %>%
        ungroup()
}

deg_buckets <- function(tbl_deg_distr) {
    #' list all buckets present in a degree distribution
    tbl_deg_distr %>%
        filter(nr_buckets > 1) %>%
        select(graph_display, nr_buckets, bucket, neighbor_deg_lb, neighbor_deg_ub) %>%
        unique()
}

## ----------------------------------------------------------------------------
## joint distributions (few buckets)
## ----------------------------------------------------------------------------

read_joint_distr <- function(graph) {
    #' joint distribution, i.e., triples of (deg_x, deg_y, count),
    #' aggregated into buckets
    read_with_graph_name("joint_histogram", graph) %>%
        filter(compl_cumul == 0) %>%
        mutate(sum = sum(count)) %>%
        group_by(center_x) %>%
        arrange(center_y, .by_group = TRUE) %>%
        mutate(
            sum_x = sum(count),
            cum_prob_y_given_x = (sum_x - cumsum(count) + count) / sum_x
        ) %>%
        ungroup() %>%
        group_by(center_y) %>%
        mutate(sum_y = sum(count)) %>%
        ungroup() %>%
        mutate(
            prob_y_given_x = count / sum_x,
            prob_y = sum_y / sum,
            factor = min(ub_x) / min(lb_x)
        )
}

joint_distr_aggr_x <- function(tbl_joint_distr) {
    #' drop all columns from the joint distribution that depend on x
    tbl_joint_distr %>%
        select(graph, ends_with("_y"), nr_buckets, factor) %>%
        unique() %>%
        arrange(center_y) %>%
        mutate(cum_prob_y = prob_y + 1 - cumsum(prob_y))
}

################################################################################
################################################################################
## formatting
################################################################################
################################################################################

## ----------------------------------------------------------------------------
## labels
## ----------------------------------------------------------------------------


deg_distr_type <- function(edge_endpoints, nr_buckets, bucket,
                           neighbor_deg_lb, neighbor_deg_ub) {
    #' type of a degree distribution: node, edge, edge [interval]
    ifelse(edge_endpoints == 0,
        "node",
        paste0(
            "edge ", ifelse(nr_buckets == 1, "", bucket / (nr_buckets - 1))
            ## , "\n[", ceiling(neighbor_deg_lb), ", ", floor(neighbor_deg_ub), "]"
        )
    )
}

## ----------------------------------------------------------------------------
## plotting
## ----------------------------------------------------------------------------

# E69F00 #56B4E9 #009E73 #F0E442 #0072B2 #D55E00 #CC79A7 #999999
colors <- palette.colors(palette = "Okabe-Ito")[-1]

scale_fill <- function(trans = "identity", ...) {
    #' setting some defaults for the fill color scheme
    scale_fill_continuous(
        trans = trans, type = "viridis",
        label = label_number(scale_cut = cut_short_scale()),
        ...
    )
}

common_formatting <- function() {
    list(
        theme( ## legend.position = "top",
            plot.caption = element_text(hjust = 0)
        ),
        facet_wrap(vars(graph_display))
    )
}



################################################################################
################################################################################
## general plot elements
################################################################################
################################################################################

log_log <- function() {
    list(
        scale_x_log10(label = label_number(scale_cut = cut_short_scale())),
        scale_y_log10(label = label_number(scale_cut = cut_short_scale())),
        annotation_logticks(
            short = unit(0.05, "cm"),
            mid = unit(0.1, "cm"),
            long = unit(0.15, "cm"),
            linewidth = 0.25)
    )
}

joint_distr_plot <- function(tbl_joint) {
    #' general setup for joint distribution plot
    ggplot(tbl_joint, aes(
        xmin = lb_x / (ub_x / lb_x)**(nr_buckets / 700), xmax = ub_x,
        ymin = lb_y / (ub_y / lb_y)**(nr_buckets / 700), ymax = ub_y
    )) +
        labs(x = "x", y = "y") +
        log_log() +
        theme(aspect.ratio = 1) +
        ## scale_fill() +
        common_formatting()
}

bucket_lines <- function(buckets) {
    #' adds arrows to a joint_distr_plot pointing to certain buckets
    buckets <- buckets %>%
        mutate(
            x_center = sqrt(neighbor_deg_lb * neighbor_deg_ub),
            factor = neighbor_deg_ub / neighbor_deg_lb,
            bucket_rel = as.character(bucket / (nr_buckets - 1))
        )

    col <- colors[-1]

    list(
        geom_segment(
            data = buckets,
            mapping = aes(
                y = x_center, yend = x_center,
                x = 1 / factor**1.8, xend = 1 / factor**0.2,
                color = factor(bucket_rel, levels = unique(bucket_rel))
            ),
            inherit.aes = FALSE,
            linewidth = 0.5,
            arrow = arrow(length = unit(0.12, "cm"))
        ),
        scale_color_manual(
            values = col, name = "",
            guide = "none"
        )
    )
}


################################################################################
################################################################################
## plots
################################################################################
################################################################################

## ----------------------------------------------------------------------------
## regular degree distribution
## ----------------------------------------------------------------------------

plot_deg_distr <- function(tbl_deg_distr) {
    ggplot(
        tbl_deg_distr,
        aes(x = degree, y = fraction_greater_equal, color = type)
    ) +
        labs(
            x = "x", y = "Pr[X ≥ x]",
            caption = paste("node: u is random node",
                "edge: {u, v} is random edge with deg(v) in range",
                sep = "\n"
            )
        ) +
        geom_step(linewidth = 0.5) +
        ## ugly hack to remove scale_y from the result of log_log()
        Filter(function(s) !("y" %in% s$aesthetics), log_log()) +
        ## remove strange bad number formatting
        scale_y_log10() +
        scale_color_manual(values = colors) +
        common_formatting()
}


## ----------------------------------------------------------------------------
## joint distribution
## ----------------------------------------------------------------------------

plot_joint <- function(tbl_joint) {
    joint_distr_plot(tbl_joint) +
        labs(
            caption = "random edge with endpoint degrees (X, Y)",
            fill = "Pr[X = x ∩ Y = y]",
        ) +
        geom_rect(aes(fill = count / sum)) +
        scale_fill_continuous(
            trans = "log10",
            labels = trans_format("log10", scientific_format()),
            type = "viridis",
            na.value = "#d9d9d9"
        )
}

## ----------------------------------------------------------------------------
## joint distribution with conditional probabilities relative to unconditional
## ----------------------------------------------------------------------------

plot_cond_rel <- function(tbl_joint) {
    joint_distr_plot(tbl_joint) +
        labs(
            caption = paste(
                "random edge with endpoint degrees (X, Y)",
                "color indicates Pr[Y = y | X = x] / Pr[Y = y]",
                sep = "\n"
            )
        ) +
        # increasing probability
        geom_rect(
            data = tbl_joint %>% filter(prob_y_given_x / prob_y >= 1),
            aes(fill = 1 - prob_y / prob_y_given_x)
        ) +
        scale_fill_gradient(
            low = "white", high = colors[6], limits = c(0, 1),
            name = "increase", guide = guide_colorbar(order = 4)
        ) +
        # different color for decreasing probability
        new_scale_fill() +
        geom_rect(
            data = tbl_joint %>% filter(prob_y_given_x / prob_y < 1),
            ## aes(fill = prob_y_given_x / prob_y)
            aes(fill = 1 - prob_y_given_x / prob_y)
        ) +
        scale_fill_gradient(
            low = "white", high = colors[5], limits = c(0, 1),
            name = "decrease", guide = guide_colorbar(order = 5)
        )
}


## ----------------------------------------------------------------------------
## joint distribution cumulative in both directions
## ----------------------------------------------------------------------------

plot_joint_cum_cum <- function(tbl_joint_cum) {
    joint_distr_plot(tbl_joint_cum) +
        labs(
            caption = "random edge with endpoint degrees (X, Y)",
            fill = "Pr[X ≥ x ∩ Y ≥ y]",
        ) +
        geom_rect(aes(fill = cum_prob)) +
        geom_contour(
            aes(x = center_x, y = center_y, z = log10(cum_prob)),
            bins = 30, color = "black"
        ) +
        scale_fill(trans = "log")
}


################################################################################
################################################################################
## running stuff
################################################################################
################################################################################

real_graphs <- function() {
    read_real_stats() %>%
        arrange(spearman) %>%
        pull(graph)
}

gen_graphs <- function(model, use_seed = 64) {
    read_gen_stats(model) %>%
        filter(gen_n == 200000, seed == use_seed) %>%
        arrange(ple, sigma) %>%
        pull(graph)
}

warm_girg_graphs <- function() {
    gen_graphs("girg", use_seed = 17)
}

print_plots <- function(graphs, file) {
    pdf(file, width = 10, height = 7)
    for (graph in graphs) {
        tbl_deg_distr <- read_deg_distr(graph)
        buckets <- deg_buckets(tbl_deg_distr)
        tbl_joint <- read_joint_distr(graph)

        print(plot_deg_distr(tbl_deg_distr) + labs(caption = NULL))
        print(plot_joint(tbl_joint) + bucket_lines(buckets) + labs(caption = NULL))
        print(plot_cond_rel(tbl_joint) + bucket_lines(buckets) + labs(caption = NULL))
    }
    dev.off()
}

print_deg_distr <- function(graph, file) {
    pdf(file, width = 3, height = 2)
    for (graph in graphs) {
        tbl_deg_distr <- read_deg_distr(graph)
        p <- plot_deg_distr(tbl_deg_distr) + labs(caption = NULL)
        print(p)
        filename <- paste0("output-individual-graphs/", graph, "-line.")
        ggsave(paste0(filename, "eps"), plot = p, width = 3, height = 2)
        ggsave(paste0(filename, "pdf"), plot = p, width = 3, height = 2)
    }
    dev.off()
}

print_joint <- function(graph, file) {
    pdf(file, width = 3, height = 2)
    for (graph in graphs) {
        tbl_deg_distr <- read_deg_distr(graph)
        buckets <- deg_buckets(tbl_deg_distr)
        tbl_joint <- read_joint_distr(graph)
        p <- plot_joint(tbl_joint) + bucket_lines(buckets) + labs(caption = NULL)
        print(p)
        filename <- paste0("output-individual-graphs/", graph, "-heatmap.")
        ggsave(paste0(filename, "eps"), plot = p, width = 2.95, height = 2)
        ggsave(paste0(filename, "pdf"), plot = p, width = 2.95, height = 2)
    }
    dev.off()
}

print_joint_change <- function(graph, file) {
    pdf(file, width = 3, height = 2)
    for (graph in graphs) {
        tbl_deg_distr <- read_deg_distr(graph)
        buckets <- deg_buckets(tbl_deg_distr)
        tbl_joint <- read_joint_distr(graph)
        p <- plot_cond_rel(tbl_joint) + bucket_lines(buckets) +
            labs(caption = NULL) +
            theme(legend.key.height = unit(0.3, "cm"))
        print(p)
        filename <- paste0("output-individual-graphs/", graph, "-cond-heatmap.")
        ggsave(paste0(filename, "eps"), plot = p, width = 2.6, height = 2)
        ggsave(paste0(filename, "pdf"), plot = p, width = 2.6, height = 2)
    }
    dev.off()
}

print_joint_change_no_legend <- function(graph, file) {
    pdf(file, width = 5, height = 5)
    for (graph in graphs) {
        tbl_deg_distr <- read_deg_distr(graph)
        buckets <- deg_buckets(tbl_deg_distr)
        tbl_joint <- read_joint_distr(graph)
        print(plot_cond_rel(tbl_joint) + bucket_lines(buckets) + labs(caption = NULL) + theme(legend.position = "none"))
    }
    dev.off()
}
