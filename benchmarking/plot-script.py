import json
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import csv
from scipy.stats import gmean

def plot_vary_img_dim(systems, unit, directory, title="Varying Dimension", x_label="Dimension", log_y="false", log_x="false", savefile="result"):

    result = None

    for system in systems:

        dim = []
        time = []
        
        print(system)
        data = open(f'{directory}/{system}', 'r')
        lines = data.readlines()

        for i in range(0, len(lines), 2):
            dim.append(float(lines[i]))
            time.append((float(lines[i+1][:-1])))
        print("DIM", dim)
        print("TIME", time)

        df = pd.DataFrame({x_label : dim, system : time})

        if result is None:
            result = df
        else:
            result = pd.merge(result, df, on=x_label, how='outer')
            # result[system] = time
    # print(result)

    full_plt = result.plot(kind = 'line', x = x_label, y = [i for i in systems],
                           marker='o',
                           title=title)
    plt.ylabel(f'Real Time ({unit})')
    
    if log_y == "true":
        plt.yscale("log")
    if log_x == "true":
        plt.xscale("log")

    plt.xlabel(x_label)
    plt.savefig(f'{directory}/{savefile}.png')

    return result

def get_speedup_up(systems, compare_to, unit, directory, title="Varying Dimension", x_label="Dimension", log_y="false", log_x="false", savefile="result"):

    result = None

    for system in systems:

        dim = []
        time = []
        
        print(system)
        data = open(f'{directory}/{system}', 'r')
        lines = data.readlines()

        for i in range(0, len(lines), 2):
            dim.append(float(lines[i]))
            time.append((float(lines[i+1][:-1])))

        df = pd.DataFrame({x_label : dim, system : time})

        if result is None:
            result = df
        else:
            result = pd.merge(result, df, on=x_label, how='outer')

    print(result)

    for system in systems:
      result[f"{system}_relative"] = result[system]/result[compare_to]
      geometric_mean = gmean(result[f"{system}_relative"])
      print(f'The geometric mean is: {system} = {geometric_mean}')

    print(result)


# get_speedup_up(["fused", "unfused"], "unfused", "s", \
#                  "/Users/manyab/TileDB/build/tiledb/examples/cpp_api/data", \
#                  title="TileDB Processing (Read + Sigmoid)", x_label="Size of DB (x times x)")

# plot_vary_img_dim(["fused", "unfused"], "s", \
#                  "/Users/manyab/TileDB/build/tiledb/examples/cpp_api/data", \
#                  title="TileDB Processing (Read + Sigmoid)", x_label="Size of DB (x times x)")

# plot_vary_img_dim(["fused", "unfused"], "s", \
#                  "./experiments-w-cmake/openvdb/data", \
#                  title="OpenVDB (Filter (Width 2), Gaussian, Mean, Median)", x_label="Number of Points")

# plot_vary_img_dim(["unsharp", "halide_unsharp"], "s", \
#                  "./experiments-w-cmake/blur-parrallel-sliding/data", \
#                  title="Unsharp", x_label="Size of Image", log_y="true",
#                  )

# plot_vary_img_dim(["fused", "unfused"], "s", \
#                  "/Users/manyab/geos/build", \
                #  title="Shapely Tree Example", x_ label="Size of Tree", log_x="true", log_y="true")

colors = {"hand" : "red", "yield" : "red", "buildit" : "blue", "unfused" : "orange",
            "ispc_fused" : "green",  "ours_8" : "blue", "halide_bench" : "blue",  "copy" : "green",
            "mine" : "green", "run-dnn-graph" : "blue", "ispc_worst_inline" : "blue", 
            "ours_no_inline" : "red", "ours_worst_inline" : "green", "unsharp" : "blue", 
            "halide_unsharp" : "black", "fused" : "green", "unfused" : "blue", 
            "internal" : "black", "materialize" : "red", "stream" : "blue", 
            "generated" : "green", "col-blocked": "green", "tiled" : "red", 
            "armpl" : "black", "accelerate": "red", "ours" : "green", 
            "ext_armpl" : "black", "ext_ours" : "green","ours_fused" :"red",
            "ext_accelerate" : "red", "ext_sgerb_ours" : "blue",  }
legend = {"hand" : "Handwritten", "yield" : "Yield", "buildit" : "BuildIt", "unfused" : "Unfused", 
        "ispc_fused" : "ISPC Implementation", "ours_8" : "Our System", "halide_bench" : "Halide", 
        "copy" : "Our System",  "mine" : "Our System", "run-dnn-graph" : "Intel OneDNN (Graph)",
        "ispc_worst_inline" : "ISPC Implementation", "ours_no_inline" : "Ours, No Inline", 
        "ours_worst_inline" : "Our System", "unsharp" : "My System", 
        "halide_unsharp" : "Halide Unsharp", "fused" : "Our System", "unfused" : "GEOS composition", 
        "internal" : "Internal", "materialize" : "Materialize", "stream" : "Stream", 
        "generated" : "Generated", "col-blocked": "Our System (Column Blocked)", 
        "tiled" : "Our System (Tiled)", "armpl" : "ArmPL (fused)", "accelerate": "Accelerate (unfused)", 
         "ours" : "Our System",  "ext_armpl" : "ArmPL (partially fused)", "ext_ours" : "Our System",
         "ext_accelerate" : "Accelerate (unfused)", "ext_sgerb_ours" : "ArmPL's sgerb with our system",
         "ours_fused": "Ours (composed with oneDNN fusion)"}
markers = {"hand" : "o", "yield" : "p", "buildit" : ".", "unfused" : "s", "ispc_fused" : "o",
            "ours_8" : "s", "halide_bench" : "o",  "copy" : "s",  "mine" : "o", "run-dnn-graph" : "s",
            "ispc_worst_inline" : "s", "ours_no_inline" : "o", 
            "ours_worst_inline" : "p", "unsharp" : "p", 
            "halide_unsharp" : "o", "fused" : "p", "unfused" : "o", 
            "internal" : "o", "materialize" : "p", "stream" : "o", 
            "generated" : "p", "col-blocked": "o", "tiled" : "p", 
            "armpl" : "p", "accelerate": "s", "ours" : "o", 
            "ext_armpl" : "p", "ext_ours" : "o",
            "ext_accelerate" : "s", "ext_sgerb_ours" : "o",
            "ours_fused" :"p"}
linestyles = {"hand" : "-", "yield" : "-", "buildit" : "--", "unfused" : "-",  
              "ispc_fused" : "-",  "ours_8" : "--", "halide_bench" : "-",  "copy" : "--",
              "mine" : "-", "run-dnn-graph" : "--",  "ispc_worst_inline" : "-", "ours_no_inline" : "--", 
              "ours_worst_inline" : "-", "unsharp" : "-", 
            "halide_unsharp" : "--", "fused" : "-", "unfused" : "--", 
            "internal" : "-", "materialize" : "--", "stream" : "-", 
            "generated" : "--", "col-blocked": "--", "tiled" : "-", 
            "armpl" : "-", "accelerate": "--", "ours" : "-.", 
            "ext_armpl" : "-", "ext_ours" : "-.",
            "ext_accelerate" : "--", "ext_sgerb_ours" : "-",
            "ours_fused" :"--"}

def make_fancy_plot(systems, unit, directory, title="Varying Dimension", x_label="Dimension", log_y="false", log_x="false", savefile="result", y_lim=0):
    result = plot_vary_img_dim(systems, unit, directory, title, x_label, log_y, log_x, savefile)
    for system in systems:
        result[legend[system]] = result[system]
    
    plt.rcParams.update({
    'font.size': 20,          # Default text size
    'axes.titlesize': 16,     # Title size
    'axes.labelsize': 20,     # Axes labels size
    'xtick.labelsize': 16,    # X-axis tick labels size
    'ytick.labelsize': 16,    # Y-axis tick labels size
    'legend.fontsize': 16,    # Legend font size
    'figure.titlesize': 16    # Figure title size
    })

    full_plt = result.plot(kind = 'line', x = x_label, y = [legend[i] for i in systems],  
                           color=[colors[system] for system in systems], 
                           #marker='plot_marker',
                           marker='o',
                        #    title=title,
                           )

    for i, line in enumerate(full_plt.get_lines()):
        line.set_marker(markers[systems[i]])
        line.set_linestyle(linestyles[systems[i]])
    
    if log_y == "true":
        plt.yscale("log")
    if log_x == "true":
        plt.xscale("log")

    plt.ylim(bottom=y_lim)


    # plt.rcParams['font.family'] = 'Serif'

    plt.legend(frameon=False, fontsize=16)

    plt.ylabel(f'Real Time ({unit})', fontsize=20)
    plt.xlabel(x_label, fontsize=20)
    
    plt.savefig(f'{directory}/{savefile}.pdf', format="pdf", bbox_inches='tight')
    plt.savefig(f'{directory}/{savefile}.png', format="png", bbox_inches='tight')
    plt.savefig(f'{directory}/{savefile}_raw_graph.svg', format="svg", bbox_inches='tight')


# make_fancy_plot(["hand", "yield", "buildit"], "s", \
#                  "/Users/manyab/meta-perf/test-examples/yield-example/build-it/data", \
#                  title="Haversine (Element-wise Fusion)", x_label="Size of Input", log_x="false", log_y="false"
#                 )

# get_speedup_up(["hand", "yield", "buildit"], "yield", "s", \
#                  "/Users/manyab/meta-perf/test-examples/yield-example/build-it/data-small", \
#                   title="Haversine (Element-wise Fusion)", x_label="Size of Input")



# get_speedup_up(["unsharp", "halide_unsharp"], "unsharp", "s", \
#                  "./experiments-w-cmake/blur-parrallel-sliding/data", \
#                  title="Conv-Bias-Relu", x_label="Size of Image", log_x="false", log_y="false",
#                  savefile="conv-bias-relu")

# get_speedup_up(["ispc_worst_inline", "ours_no_inline", "ours_worst_inline"], "ours_worst_inline", "s", \
#                  "./my_libs/array_lib/data", \
#                  title="Conv-Bias-Relu", x_label="Size of Image", log_x="false", log_y="false",
#                  savefile="conv-bias-relu")

# make_fancy_plot(["unsharp", "halide_unsharp"], "s", \
#                  "./experiments-w-cmake/blur-parrallel-sliding/data", \
#                  title="Unsharp", x_label="Size of Image", log_y="true",
#                  )

# make_fancy_plot(["fused", "unfused"], "s", \
#                  "/Users/manyab/TileDB/build/tiledb/examples/cpp_api/data", \
#                  title="TileDB Processing (Read + Sigmoid)", x_label="Size of DB (x times x)")

# make_fancy_plot(["fused", "unfused"], "s", \
#                  "/Users/manyab/opencl-experiments/cuda/data", \
#                  title="Worst Case", x_label="Size of Array", log_y="true",
#                  )





# FINAL

# get_speedup_up(["mine", "run-dnn-graph"], "mine", "s", \
#                  "./my_libs/l-m-conv-max/data", \
#                  title="Convolutional Layer", x_label="Size of Image", log_x="false", log_y="false",
#                  savefile="conv-bias-relu")

# make_fancy_plot(["mine", "run-dnn-graph"], "s", \
#                  "./my_libs/l-m-conv-max/data", \
#                  title="Convolutional Layer", x_label="Image Dimension (n)", log_x="false", log_y="false",
#                  savefile="conv-bias-relu")

# make_fancy_plot(["ispc_fused", "ours_8"], "s", \
#                  "./my_libs/array_lib/data", \
#                  title="Haversine", x_label="Size of Array",
#                 savefile="haversine"
#                  )

# get_speedup_up(["ispc_fused", "ours_8"], "ours_8", "s", \
#                  "./my_libs/array_lib/data", \
#                  title="Haversine", x_label="Size of Array",
#                 savefile="haversine"
#                  )

# make_fancy_plot(["halide_bench", "copy"], "s", \
#                  "./experiments-w-cmake/blur-parrallel-sliding/data", \
#                  title="Halide Blur X, Blur Y (Sliding Window)", 
#                  x_label="Image Dimension (n)",
#                  savefile="blur2d"
#                  )

# get_speedup_up(["halide_bench", "copy"], "copy", "s", \
#                  "./experiments-w-cmake/blur-parrallel-sliding/data", \
#                  title="Halide Blur X, Blur Y (Sliding Window)", 
#                  x_label="Image Dimension (n)",
#                  savefile="blur2d"
#                  )


# make_fancy_plot(["fused", "unfused"], "s", \
#                  "/Users/manyab/geos", \
#                  title="Haversine", x_label="Number of Geometries", log_x="true",
                
#                 savefile="shapely-bench"
#                  )


# get_speedup_up(["fused", "unfused"], "fused", "s", \
#                  "/Users/manyab/geos", \
#                  title="Haversine", x_label="Number of Geometries",
#                 savefile="shapely-bench", 
#                  )


# make_fancy_plot(["ispc_worst_inline", "ours_worst_inline"], "s", \
#                  "./my_libs/array_lib/data", \
#                  title="Haversine", x_label="Size of Array",
#                 savefile="worst-case"
#                  )

# get_speedup_up(["ispc_worst_inline", "ours_worst_inline"], 
#                 "ours_worst_inline", "s", \
#                  "./my_libs/array_lib/data", \
#                  title="Haversine", x_label="Size of Array",
#                 savefile="worst-case"
#                  )


# make_fancy_plot(["armpl", "accelerate", "ours"], "s", \
#                  "/Users/manyab/meta-perf/my_libs/arm_pl/data", \
#                  title="sgerb + A + A (alpha * x * y + beta + A + A + A)", 
#                  x_label="Vector Length (n)", log_y="false",
#                  savefile="sgerb"
#                 )

# get_speedup_up(["armpl", "accelerate", "ours"], "armpl", "s", \
#                  "/Users/manyab/meta-perf/my_libs/arm_pl/data", \
#                  title="sgerb + A + A (alpha * x * y + beta + A + A + A)", 
#                  x_label="Vector Length (n)", log_y="false",
#                  savefile="sgerb"
#                 )

# make_fancy_plot(["ext_armpl", "ext_ours", "ext_accelerate" , "ext_sgerb_ours"], "s", \
#                  "/Users/manyab/meta-perf/my_libs/arm_pl/data", \
#                  title="sgerb + B + C (alpha * x * y + beta + A + B + C)", 
#                  x_label="Vector Length (n)", log_y="false",
#                  savefile="sgerb_ext"
#                 )

# get_speedup_up(["ext_armpl", "ext_ours", "ext_accelerate" , "ext_sgerb_ours"], \
#                  "ext_sgerb_ours", "s", \
#                  "/Users/manyab/meta-perf/my_libs/arm_pl/data", \
#                  title="sgerb + A + A (alpha * x * y + beta + A + A + A)", 
#                  x_label="Vector Length (n)", log_y="false",
#                  savefile="sgerb_ext"
#                 )

# get_speedup_up(["ours", "run-dnn-graph", "ours_fused"], "ours_fused", "s", \
#                  "./my_libs/l-m-conv-max/data_max", \
#                  title="Convolutional Layer -> Maxpool", x_label="Size of Image", log_x="false", log_y="false",
#                  savefile="conv-bias-relu-max")

# make_fancy_plot(["ours", "run-dnn-graph", "ours_fused"], "s", \
#                  "./my_libs/l-m-conv-max/data_max", \
#                  title="Convolutional Layer -> Maxpool", x_label="Image Dimension (n)", log_x="false", log_y="false",
#                  savefile="conv-bias-relu-max")
make_fancy_plot(["hand", "fused"], "ms", "./matmul_data", title="Varying Tile Size", x_label="Tile Size", y_lim=800)
make_fancy_plot(["hand", "fused"], "ms", "./attention_data", title="Varying Tile Size", x_label="Tile Size", y_lim=1200)