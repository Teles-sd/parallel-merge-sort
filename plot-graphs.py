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
distributedCSVFilename = "distributed-tests.csv"

sequentialCSVPath = os.path.join(scriptDir, sequentialCSVFilename)
parallelCSVPath = os.path.join(scriptDir, parallelCSVFilename)
distributedCSVPath = os.path.join(scriptDir, distributedCSVFilename)

# Data
if os.path.isfile(sequentialCSVPath) and os.path.isfile(parallelCSVPath) and os.path.isfile(distributedCSVPath):

    df_parallel = pd.read_csv(parallelCSVPath, index_col='Run')
    df_distributed = pd.read_csv(distributedCSVPath, index_col='Run')
    

    # OMP
    
    df_sequential = pd.read_csv(sequentialCSVPath, index_col='Run')
    df_sequential["Num_threads"] = 1
    df_all_omp = pd.concat([df_parallel, df_sequential], ignore_index=True)
    # time_dtype = df_all_omp["Time_ms"].dtype

    df_group_omp = df_all_omp.groupby(["Num_threads", "Array_size"])
    df_group_mean_omp = df_group_omp.mean()
    df_group_std_omp = df_group_omp.std()
    numExecutions = df_group_omp.count()["Time_ms"].iloc[0]

    x_arraySizes_omp = df_group_mean_omp.index.get_level_values("Array_size").unique().to_numpy(copy=True)
    x_numThreads_omp = df_group_mean_omp.index.get_level_values("Num_threads").unique().to_numpy(copy=True)

    y_timeMean_byNumThreads_omp = {int(threads):df_group_mean_omp.xs(threads, level="Num_threads").to_numpy(copy=True).flatten()  for threads in x_numThreads_omp}
    y_timeStd_byNumThreads_omp  = {int(threads):df_group_std_omp.xs(threads, level="Num_threads").to_numpy(copy=True).flatten()   for threads in x_numThreads_omp}
    
    y_timeMean_byArraySizes_omp = {int(size):df_group_mean_omp.xs(size, level="Array_size").to_numpy(copy=True).flatten()     for size in x_arraySizes_omp}
    y_timeStd_byArraySizes_omp  = {int(size):df_group_std_omp.xs(size, level="Array_size").to_numpy(copy=True).flatten()      for size in x_arraySizes_omp}
    
    # MPI
    
    df_sequential = pd.read_csv(sequentialCSVPath, index_col='Run')
    df_sequential["Num_procs"] = 1
    df_all_mpi = pd.concat([df_distributed, df_sequential], ignore_index=True)

    df_group_mpi = df_all_mpi.groupby(["Num_procs", "Array_size"])
    df_group_mean_mpi = df_group_mpi.mean()
    df_group_std_mpi = df_group_mpi.std()

    x_arraySizes_mpi = df_group_mean_mpi.index.get_level_values("Array_size").unique().to_numpy(copy=True)
    x_numProcess_mpi = df_group_mean_mpi.index.get_level_values("Num_procs").unique().to_numpy(copy=True)

    y_timeMean_byNumProcess_mpi = {int(threads):df_group_mean_mpi.xs(threads, level="Num_procs").to_numpy(copy=True).flatten()  for threads in x_numProcess_mpi}
    y_timeStd_byNumProcess_mpi  = {int(threads):df_group_std_mpi.xs(threads, level="Num_procs").to_numpy(copy=True).flatten()   for threads in x_numProcess_mpi}
    
    y_timeMean_byArraySizes_mpi = {int(size):df_group_mean_mpi.xs(size, level="Array_size").to_numpy(copy=True).flatten()     for size in x_arraySizes_mpi}
    y_timeStd_byArraySizes_mpi  = {int(size):df_group_std_mpi.xs(size, level="Array_size").to_numpy(copy=True).flatten()      for size in x_arraySizes_mpi}

else:
    print("Something's missing.")
    exit()





# Separating tick labels that will be displayed linearly and logarithmically
x1_tickLabels_lin = x_arraySizes_omp[3:]
x1_tickLabels_log = x_arraySizes_omp[:3]

y1_tickLabels_lin = np.concatenate( [arr[3:] for arr in y_timeMean_byNumThreads_omp.values()] )
y1_tickLabels_log = np.concatenate( [arr[:3] for arr in y_timeMean_byNumThreads_omp.values()] )


# OMP Figure: time vs size
figFilename = "omp_size-time"
(fig1, (lg1, ax1)) = plt.subplots(1, 2, facecolor = 'w', figsize=(15,7))

fig1.suptitle(f"Tempo de Execução: Média e Desvio Padrão ({numExecutions} execuções)")
def plot1():
    ax1.set_title("Tempo em escala linear")
    lg1.set_title("Tempo em escala logarítmica")

    for nThreads,format in zip(x_numThreads_omp, [".--k", "*--r", "2--y", "s--g", "p--b", "o--m"]):
        ax1.errorbar(x_arraySizes_omp[3:], y_timeMean_byNumThreads_omp[nThreads][3:], yerr=y_timeStd_byNumThreads_omp[nThreads][3:], fmt=format, lw=2, label=f"{nThreads} threads", capsize=7)
        lg1.errorbar(x_arraySizes_omp[:3], y_timeMean_byNumThreads_omp[nThreads][:3], yerr=y_timeStd_byNumThreads_omp[nThreads][:3], fmt=format, lw=2, label=f"{nThreads} threads", capsize=7)


    lg1.set_ylabel("Tempo (milissegundos)")
    lg1.set_yscale('log')
    lg1.set_yticks(y1_tickLabels_log, labels=np.round(y1_tickLabels_log, decimals=4))

    lg1.set_xlabel("Tamanho do Array")
    lg1.set_xscale('log')
    lg1.set_xticks(x1_tickLabels_log, labels=x1_tickLabels_log)
    lg1.tick_params(axis="x", rotation=30)

    ax1.set_xlabel("Tamanho do Array")
    ax1.set_xscale('log')
    ax1.set_xticks(x1_tickLabels_lin, labels=x1_tickLabels_lin)
    ax1.tick_params(axis="x", rotation=30)

    ax1.set_yticks(y1_tickLabels_lin, labels=np.round(y1_tickLabels_lin, decimals=4))


    ax1.grid()
    lg1.grid()

    ax1.legend()
    lg1.legend()
plot1()
fig1.tight_layout()
fig1.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")




# Separating tick labels that will be displayed linearly and logarithmically
y2_tickLabels_lin = np.concatenate( [arr for arr in y_timeMean_byNumThreads_omp.values()] )


# OMP Figure: time vs threads
figFilename = "omp_threads-time"
(fig2, ax2) = plt.subplots(1, 1, facecolor = 'w', figsize=(13,7))

fig2.suptitle(f"Tempo de Execução: Média e Desvio Padrão ({numExecutions} execuções)")
def plot2():
    for size,fmtString in zip(x_arraySizes_omp, [".--r", "*--y", "2--g", "s--c", "p--b", "o--m"]):
        ax2.errorbar(x_numThreads_omp, y_timeMean_byArraySizes_omp[size], yerr=y_timeStd_byArraySizes_omp[size], fmt=fmtString, lw=2.7, label=f"Tamanho do Array: {size}", capsize=8)


    ax2.set_ylabel("Tempo (milissegundos)")
    ax2.set_yticks(y2_tickLabels_lin, labels=np.round(y2_tickLabels_lin, decimals=4))

    ax2.set_xlabel("Threads")
    # ax2.set_xticks(x_numThreads_omp, labels=x_numThreads_omp)
    ax2.set_xticks(x_numThreads_omp)


    ax2.grid()
    ax2.legend()
plot2()
fig2.tight_layout()
fig2.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")





# Separating tick labels that will be displayed linearly and logarithmically
x3_tickLabels_lin = x_arraySizes_mpi[3:]
x3_tickLabels_log = x_arraySizes_mpi[:3]

y3_tickLabels_lin = np.concatenate( [arr[3:] for arr in y_timeMean_byNumProcess_mpi.values()] )
y3_tickLabels_log = np.concatenate( [arr[:3] for arr in y_timeMean_byNumProcess_mpi.values()] )


# MPI Figure: time vs size
figFilename = "mpi_size-time"
(fig3, (lg3, ax3)) = plt.subplots(1, 2, facecolor = 'w', figsize=(15,7))

fig3.suptitle(f"Tempo de Execução: Média e Desvio Padrão ({numExecutions} execuções)")
def plot3():
    ax3.set_title("Tempo em escala linear")
    lg3.set_title("Tempo em escala logarítmica")

    for nProcess,format in zip(x_numProcess_mpi, [".--k", "*--r", "2--y", "s--g", "p--b", "o--m"]):
        ax3.errorbar(x_arraySizes_mpi[3:], y_timeMean_byNumProcess_mpi[nProcess][3:], yerr=y_timeStd_byNumProcess_mpi[nProcess][3:], fmt=format, lw=2, label=f"{nProcess} processos", capsize=7)
        lg3.errorbar(x_arraySizes_mpi[:3], y_timeMean_byNumProcess_mpi[nProcess][:3], yerr=y_timeStd_byNumProcess_mpi[nProcess][:3], fmt=format, lw=2, label=f"{nProcess} processos", capsize=7)


    lg3.set_ylabel("Tempo (milissegundos)")
    lg3.set_yscale('log')
    lg3.set_yticks(y3_tickLabels_log, labels=np.round(y3_tickLabels_log, decimals=4))

    lg3.set_xlabel("Tamanho do Array")
    lg3.set_xscale('log')
    lg3.set_xticks(x3_tickLabels_log, labels=x3_tickLabels_log)
    lg3.tick_params(axis="x", rotation=30)

    ax3.set_xlabel("Tamanho do Array")
    ax3.set_xscale('log')
    ax3.set_xticks(x3_tickLabels_lin, labels=x3_tickLabels_lin)
    ax3.tick_params(axis="x", rotation=30)

    ax3.set_yticks(y3_tickLabels_lin, labels=np.round(y3_tickLabels_lin, decimals=4))


    ax3.grid()
    lg3.grid()

    ax3.legend()
    lg3.legend()
plot3()
fig3.tight_layout()
fig3.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")




# Separating tick labels that will be displayed linearly and logarithmically
y4_tickLabels_lin = np.concatenate( [arr for arr in y_timeMean_byNumProcess_mpi.values()] )


# MPI Figure: time vs process
figFilename = "mpi_process-time"
(fig4, ax4) = plt.subplots(1, 1, facecolor = 'w', figsize=(13,7))

fig4.suptitle(f"Tempo de Execução: Média e Desvio Padrão ({numExecutions} execuções)")
def plot4():
    for size,fmtString in zip(x_arraySizes_mpi, [".--r", "*--y", "2--g", "s--c", "p--b", "o--m"]):
        ax4.errorbar(x_numProcess_mpi, y_timeMean_byArraySizes_mpi[size], yerr=y_timeStd_byArraySizes_mpi[size], fmt=fmtString, lw=2.7, label=f"Tamanho do Array: {size}", capsize=8)


    ax4.set_ylabel("Tempo (milissegundos)")
    ax4.set_yticks(y4_tickLabels_lin, labels=np.round(y4_tickLabels_lin, decimals=4))

    ax4.set_xlabel("Processos")
    ax4.set_xticks(x_numProcess_mpi)


    ax4.grid()
    ax4.legend()
plot4()
fig4.tight_layout()
fig4.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")




# Comparison Figure: time vs size
figFilename = "compare_size-time"
( fig5, ((lg1, ax1), (lg3, ax3)) ) = plt.subplots(2, 2, facecolor = 'w', figsize=(13,10))
fig5.suptitle(f"Comparação de Tempo de Execução: OpenMP (acima) e OpenMPI (abaixo)")
plot1()
plot3()
fig5.tight_layout()
fig5.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")




# Comparison Figure: time vs process
figFilename = "compare_process-time"
( fig6, (ax2, ax4) ) = plt.subplots(2, 1, facecolor = 'w', figsize=(13,9))
fig6.suptitle(f"Comparação de Tempo de Execução: OpenMP (acima) e OpenMPI (abaixo)")
plot2()
plot4()
fig6.tight_layout()
fig6.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")