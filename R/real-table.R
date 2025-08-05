source("helper/load_helpers.R")

library("xtable")

num_format <- function(x) {
    if (is.numeric(x)) {
        paste0("\\num{", round(x, 2), "}")
    } else {
        x
    }
}

tbl <- read_real_stats() %>%
    mutate(avg_deg = 2 * m / n) %>%
    select(graph = konect_display_name, n, m, avg_deg, tau = ple_hill, assortativity = spearman) %>%
    arrange(-assortativity)
tbl

xtbl <- xtable(data.frame(lapply(tbl, num_format)),
    align = c("l", "l", "r", "r", "r", "r", "r"))
names(xtbl) <- c("graph", "vertices", "edges", "avg degree", "$\\tau$", "assortativity")

print(xtbl,
      type = "latex",
      file = "output/real-table.tex",
      include.rownames = FALSE,
      sanitize.colnames.function = identity,
      sanitize.text.function = identity,
      booktabs = TRUE
)
