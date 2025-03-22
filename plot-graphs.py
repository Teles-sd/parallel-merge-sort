import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

import os

# Directories: of the script being run and where the pictures are saved.
scriptDir = os.path.dirname(os.path.abspath(__file__))
picSaveDir = os.path.join(scriptDir, "Pictures")

# CSV files
sequentialCSVFilename = "sequential-tests.csv"
parallelCSVFilename = "parallel-tests.csv"

sequentialCSVPath = os.path.join(scriptDir, sequentialCSVFilename)
parallelCSVPath = os.path.join(scriptDir, parallelCSVFilename)

# Data
if os.path.isfile(sequentialCSVPath) and os.path.isfile(parallelCSVFilename):

    df_sequential = pd.read_csv(sequentialCSVPath, index_col='Run')
    df_parallel = pd.read_csv(parallelCSVFilename, index_col='Run')
    
    df_sequential["Num_threads"] = 1
    
    df_all = pd.concat([df_parallel, df_sequential], ignore_index=True)
    time_dtype = df_all["Time_ms"].dtype

    df_group = df_all.groupby(["Num_threads", "Array_size"])
    df_group_mean = df_group.mean()
    df_group_std = df_group.std()
    numExecutions = df_group.count()["Time_ms"].iloc[0]

    x_arraySizes = df_group_mean.index.get_level_values("Array_size").unique().to_numpy(copy=True)
    x_numThreads = df_group_mean.index.get_level_values("Num_threads").unique().to_numpy(copy=True)

    y_timeMean_byNumThreads = {int(threads):df_group_mean.xs(threads, level="Num_threads").to_numpy(copy=True).flatten()  for threads in x_numThreads}
    y_timeStd_byNumThreads  = {int(threads):df_group_std.xs(threads, level="Num_threads").to_numpy(copy=True).flatten()   for threads in x_numThreads}
    
    y_timeMean_byArraySizes = {int(size):df_group_mean.xs(size, level="Array_size").to_numpy(copy=True).flatten()     for size in x_arraySizes}
    y_timeStd_byArraySizes  = {int(size):df_group_std.xs(size, level="Array_size").to_numpy(copy=True).flatten()      for size in x_arraySizes}
    
else:
    print("Something's missing.")
    exit()



# Separating tick labels that will be displayed linearly and logarithmically
x_tickLabels_lin = x_arraySizes[3:]
x_tickLabels_log = x_arraySizes[:3]

y_tickLabels_lin = np.concatenate( [arr[3:] for arr in y_timeMean_byNumThreads.values()] )
y_tickLabels_log = np.concatenate( [arr[:3] for arr in y_timeMean_byNumThreads.values()] )



# Figure: time vs size
figName = "size-time"
(fig, (lg, ax)) = plt.subplots(1, 2, facecolor = 'w', figsize=(15,7))

fig.suptitle(f"Tempo de Execução: Média e Desvio Padrão ({numExecutions} execuções)")

ax.set_title("Tempo em escala linear")
lg.set_title("Tempo em escala logarítmica")

for nThreads,format in zip(x_numThreads, [".--k", "*--r", "2--y", "s--g", "p--b", "o--m"]):
    ax.errorbar(x_arraySizes[3:], y_timeMean_byNumThreads[nThreads][3:], yerr=y_timeStd_byNumThreads[nThreads][3:], fmt=format, lw=2, label=f"{nThreads} threads", capsize=7)
    lg.errorbar(x_arraySizes[:3], y_timeMean_byNumThreads[nThreads][:3], yerr=y_timeStd_byNumThreads[nThreads][:3], fmt=format, lw=2, label=f"{nThreads} threads", capsize=7)


lg.set_ylabel("Tempo (milissegundos)")
lg.set_yscale('log')
lg.set_yticks(y_tickLabels_log, labels=np.round(y_tickLabels_log, decimals=4))

lg.set_xlabel("Tamanho do Array")
lg.set_xscale('log')
lg.set_xticks(x_tickLabels_log, labels=x_tickLabels_log)
lg.tick_params(axis="x", rotation=30)

ax.set_xlabel("Tamanho do Array")
ax.set_xscale('log')
ax.set_xticks(x_tickLabels_lin, labels=x_tickLabels_lin)
ax.tick_params(axis="x", rotation=30)

ax.set_yticks(y_tickLabels_lin, labels=np.round(y_tickLabels_lin, decimals=4))


ax.grid()
lg.grid()

ax.legend()
lg.legend()

fig.tight_layout()
fig.savefig(f"{os.path.join(picSaveDir, figName)}.png")




# Separating tick labels that will be displayed linearly and logarithmically
y_tickLabels_lin = np.concatenate( [arr for arr in y_timeMean_byNumThreads.values()] )


# Figure: time vs threads
figName = "threads-time"
(fig, ax) = plt.subplots(1, 1, facecolor = 'w', figsize=(13,7))

fig.suptitle(f"Tempo de Execução: Média e Desvio Padrão ({numExecutions} execuções)")

for size,fmtString in zip(x_arraySizes, [".--r", "*--y", "2--g", "s--c", "p--b", "o--m"]):
    ax.errorbar(x_numThreads, y_timeMean_byArraySizes[size], yerr=y_timeStd_byArraySizes[size], fmt=fmtString, lw=2.7, label=f"Tamanho do Array: {size}", capsize=8)


ax.set_ylabel("Tempo (milissegundos)")
ax.set_yticks(y_tickLabels_lin, labels=np.round(y_tickLabels_lin, decimals=4))

ax.set_xlabel("Threads")
# ax.set_xticks(x_numThreads, labels=x_numThreads)
ax.set_xticks(x_numThreads)


ax.grid()
ax.legend()

fig.tight_layout()
fig.savefig(f"{os.path.join(picSaveDir, figName)}.png")