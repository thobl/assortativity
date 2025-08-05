source("degree_distr.R")

graphs <- real_graphs()
print_deg_distr(graphs, "output/deg_distr_real.pdf")
print_joint(graphs, "output/deg_distr_real_joint.pdf")
print_joint_change(graphs, "output/deg_distr_real_change.pdf")

graphs <- gen_graphs("cl")
print_deg_distr(graphs, "output/deg_distr_cl.pdf")
print_joint(graphs, "output/deg_distr_cl_joint.pdf")
print_joint_change(graphs, "output/deg_distr_cl_change.pdf")

graphs <- gen_graphs("girg")
print_deg_distr(graphs, "output/deg_distr_girg.pdf")
print_joint(graphs, "output/deg_distr_girg_joint.pdf")
print_joint_change(graphs, "output/deg_distr_girg_change.pdf")

graphs <- warm_girg_graphs()
print_deg_distr(graphs, "output/deg_distr_girg_warm.pdf")
print_joint(graphs, "output/deg_distr_girg_warm_joint.pdf")
print_joint_change(graphs, "output/deg_distr_girg_warm_change.pdf")
