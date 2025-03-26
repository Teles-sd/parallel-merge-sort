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

    df_group_omp = df_all_omp.groupby(["Num_threads", "Array_size"])
    df_group_mean_omp = df_group_omp.mean()
    df_group_std_omp = df_group_omp.std()
    numExecutions = df_group_omp.count()["Time_ms"].iloc[0]

    x_arraySizes_omp = df_group_mean_omp.index.get_level_values("Array_size").unique().to_numpy(copy=True)
    x_numThreads_omp = df_group_mean_omp.index.get_level_values("Num_threads").unique().to_numpy(copy=True)

    y_timeMean_byNumThreads_omp = {int(nThreads):df_group_mean_omp.xs(nThreads, level="Num_threads").to_numpy(copy=True).flatten()  for nThreads in x_numThreads_omp}
    y_timeStd_byNumThreads_omp  = {int(nThreads):df_group_std_omp.xs(nThreads, level="Num_threads").to_numpy(copy=True).flatten()   for nThreads in x_numThreads_omp}
    
    y_timeMean_byArraySizes_omp = {int(size):df_group_mean_omp.xs(size, level="Array_size").to_numpy(copy=True).flatten()     for size in x_arraySizes_omp}
    y_timeStd_byArraySizes_omp  = {int(size):df_group_std_omp.xs(size, level="Array_size").to_numpy(copy=True).flatten()      for size in x_arraySizes_omp}
    
    df_group_metric_omp = df_group_mean_omp.copy()
    df_group_metric_omp["Speedup"] = pd.concat( [ df_group_metric_omp.xs(1, level="Num_threads") ]*len(x_numThreads_omp) )["Time_ms"].to_numpy(copy=True).flatten()  /  df_group_metric_omp["Time_ms"].to_numpy(copy=True).flatten()
    df_group_metric_omp["Efficiency"] = df_group_metric_omp["Speedup"] / df_group_metric_omp.index.droplevel("Array_size")
    df_group_metric_omp["Performance"] = df_group_metric_omp["Efficiency"] * 100
    # print(df_group_metric_omp)
    
    y_speedup_byArraySizes_omp = {int(size):df_group_metric_omp.xs(size, level="Array_size")["Speedup"].to_numpy(copy=True).flatten()     for size in x_arraySizes_omp}
    y_performance_byArraySizes_omp = {int(size):df_group_metric_omp.xs(size, level="Array_size")["Performance"].to_numpy(copy=True).flatten()     for size in x_arraySizes_omp}
    
    # MPI
    
    df_sequential = pd.read_csv(sequentialCSVPath, index_col='Run')
    df_sequential["Num_procs"] = 1
    df_all_mpi = pd.concat([df_distributed, df_sequential], ignore_index=True)

    df_group_mpi = df_all_mpi.groupby(["Num_procs", "Array_size"])
    df_group_mean_mpi = df_group_mpi.mean()
    df_group_std_mpi = df_group_mpi.std()

    x_arraySizes_mpi = df_group_mean_mpi.index.get_level_values("Array_size").unique().to_numpy(copy=True)
    x_numProcess_mpi = df_group_mean_mpi.index.get_level_values("Num_procs").unique().to_numpy(copy=True)

    y_timeMean_byNumProcess_mpi = {int(nProcess):df_group_mean_mpi.xs(nProcess, level="Num_procs").to_numpy(copy=True).flatten()  for nProcess in x_numProcess_mpi}
    y_timeStd_byNumProcess_mpi  = {int(nProcess):df_group_std_mpi.xs(nProcess, level="Num_procs").to_numpy(copy=True).flatten()   for nProcess in x_numProcess_mpi}
    
    y_timeMean_byArraySizes_mpi = {int(size):df_group_mean_mpi.xs(size, level="Array_size").to_numpy(copy=True).flatten()     for size in x_arraySizes_mpi}
    y_timeStd_byArraySizes_mpi  = {int(size):df_group_std_mpi.xs(size, level="Array_size").to_numpy(copy=True).flatten()      for size in x_arraySizes_mpi}

    df_group_metric_mpi = df_group_mean_mpi.copy()
    df_group_metric_mpi["Speedup"] = pd.concat( [ df_group_metric_mpi.xs(1, level="Num_procs") ]*len(x_numProcess_mpi) )["Time_ms"].to_numpy(copy=True).flatten()  /  df_group_metric_mpi["Time_ms"].to_numpy(copy=True).flatten()
    df_group_metric_mpi["Efficiency"] = df_group_metric_mpi["Speedup"] / df_group_metric_mpi.index.droplevel("Array_size")
    df_group_metric_mpi["Performance"] = df_group_metric_mpi["Efficiency"] * 100
    
    y_speedup_byArraySizes_mpi = {int(size):df_group_metric_mpi.xs(size, level="Array_size")["Speedup"].to_numpy(copy=True).flatten()     for size in x_arraySizes_mpi}
    y_performance_byArraySizes_mpi = {int(size):df_group_metric_mpi.xs(size, level="Array_size")["Performance"].to_numpy(copy=True).flatten()     for size in x_arraySizes_mpi}
    
else:
    print("Something's missing.")
    exit()





# Separating tick labels that will be displayed linearly and logarithmically
x1_tickLabels_log = x_arraySizes_omp[:3]
x1_tickLabels_lin = x_arraySizes_omp[3:]

y1_tickLabels_log = np.concatenate( [arr[:3] for arr in y_timeMean_byNumThreads_omp.values()] )
y1_tickLabels_lin = np.concatenate( [arr[4:] for arr in y_timeMean_byNumThreads_omp.values()] )


# OMP Figure: time vs size
figFilename = "omp_size-time"
(fig1, (lg1, ax1)) = plt.subplots(1, 2, facecolor = 'w', figsize=(15,7))

fig1.suptitle(f"Tempo de Execução: Média e Desvio Padrão ({numExecutions} execuções)")
def plot1():
    ax1.set_title("Tempo em escala linear")
    lg1.set_title("Tempo em escala logarítmica")

    for nThreads,fmtString in zip(x_numThreads_omp, [".--k", "*--r", "2--y", "s--g", "p--b", "o--m"]):
        ax1.errorbar(x_arraySizes_omp[3:], y_timeMean_byNumThreads_omp[nThreads][3:], yerr=y_timeStd_byNumThreads_omp[nThreads][3:], fmt=fmtString, lw=2, label="Sequencial" if nThreads==1 else f"{nThreads} nThreads", capsize=7)
        lg1.errorbar(x_arraySizes_omp[:3], y_timeMean_byNumThreads_omp[nThreads][:3], yerr=y_timeStd_byNumThreads_omp[nThreads][:3], fmt=fmtString, lw=2, label="Sequencial" if nThreads==1 else f"{nThreads} nThreads", capsize=7)


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
# y2_tickLabels_lin = np.concatenate( [arr for arr in y_timeMean_byArraySizes_omp.values()] )
y2_tickLabels_lin = np.concatenate( [arr for arr in list(y_timeMean_byArraySizes_omp.values())[-2:]] )


# OMP Figure: time vs nThreads
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
y3_tickLabels_log = np.concatenate( [arr for arr in list(y_speedup_byArraySizes_omp.values())[2:]] )
y3_tickLabels_lin = np.concatenate( [arr for arr in y_performance_byArraySizes_omp.values()] )


# OMP Figure: speedup
figFilename = "omp_speedup"
(fig3, (lg3, ax3)) = plt.subplots(1, 2, facecolor = 'w', figsize=(15,7))

fig3.suptitle(f"Speedup e Desempenho médio ({numExecutions} execuções)")
def plot3():
    lg3.set_title("Speedup")
    ax3.set_title("Desempenho")

    lg3.plot(x_numThreads_omp, [1 for ugh in x_numThreads_omp], ":k", lw=2.7)
    ax3.plot(x_numThreads_omp, [100 for ugh in x_numThreads_omp], ":k", lw=2.7)

    for size,fmtString in zip(x_arraySizes_omp, [".--r", "*--y", "2--g", "s--c", "p--b", "o--m"]):
        lg3.plot(x_numThreads_omp, y_speedup_byArraySizes_omp[size], fmtString, lw=2, label=f"Tamanho do Array: {size}")
        ax3.plot(x_numThreads_omp, y_performance_byArraySizes_omp[size], fmtString, lw=2, label=f"Tamanho do Array: {size}")


    lg3.set_xlabel("Threads")
    lg3.set_xticks(x_numThreads_omp, labels=x_numThreads_omp)

    lg3.set_yticks(y3_tickLabels_log, labels=np.round(y3_tickLabels_log, decimals=3))

    ax3.set_xlabel("Threads")
    ax3.set_xticks(x_numThreads_omp, labels=x_numThreads_omp)

    ax3.set_yticks(y3_tickLabels_lin, labels=[f"{prctg} %" for prctg in np.round(y3_tickLabels_lin, decimals=2)])


    lg3.grid()
    ax3.grid()

    lg3.legend()
    ax3.legend()
plot3()
fig3.tight_layout()
fig3.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")





# Separating tick labels that will be displayed linearly and logarithmically
x4_tickLabels_log = x_arraySizes_mpi[:3]
x4_tickLabels_lin = x_arraySizes_mpi[3:]

y4_tickLabels_log = np.concatenate( [arr[:3] for arr in y_timeMean_byNumProcess_mpi.values()] )
y4_tickLabels_lin = np.concatenate( [arr[4:] for arr in y_timeMean_byNumProcess_mpi.values()] )


# MPI Figure: time vs size
figFilename = "mpi_size-time"
(fig4, (lg4, ax4)) = plt.subplots(1, 2, facecolor = 'w', figsize=(15,7))

fig4.suptitle(f"Tempo de Execução: Média e Desvio Padrão ({numExecutions} execuções)")
def plot4():
    ax4.set_title("Tempo em escala linear")
    lg4.set_title("Tempo em escala logarítmica")

    for nProcess,fmtString in zip(x_numProcess_mpi, [".--k", "*--r", "2--y", "s--g", "p--b", "o--m"]):
        lg4.errorbar(x_arraySizes_mpi[:3], y_timeMean_byNumProcess_mpi[nProcess][:3], yerr=y_timeStd_byNumProcess_mpi[nProcess][:3], fmt=fmtString, lw=2, label="Sequencial" if nProcess==1 else f"{nProcess} processos", capsize=7)
        ax4.errorbar(x_arraySizes_mpi[3:], y_timeMean_byNumProcess_mpi[nProcess][3:], yerr=y_timeStd_byNumProcess_mpi[nProcess][3:], fmt=fmtString, lw=2, label="Sequencial" if nProcess==1 else f"{nProcess} processos", capsize=7)


    lg4.set_ylabel("Tempo (milissegundos)")
    lg4.set_yscale('log')
    lg4.set_yticks(y4_tickLabels_log, labels=np.round(y4_tickLabels_log, decimals=4))

    lg4.set_xlabel("Tamanho do Array")
    lg4.set_xscale('log')
    lg4.set_xticks(x4_tickLabels_log, labels=x4_tickLabels_log)
    lg4.tick_params(axis="x", rotation=30)

    ax4.set_xlabel("Tamanho do Array")
    ax4.set_xscale('log')
    ax4.set_xticks(x4_tickLabels_lin, labels=x4_tickLabels_lin)
    ax4.tick_params(axis="x", rotation=30)

    ax4.set_yticks(y4_tickLabels_lin, labels=np.round(y4_tickLabels_lin, decimals=4))


    ax4.grid()
    lg4.grid()

    ax4.legend()
    lg4.legend()
plot4()
fig4.tight_layout()
fig4.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")




# Separating tick labels that will be displayed linearly and logarithmically
y5_tickLabels_lin = np.concatenate( [arr for arr in list(y_timeMean_byArraySizes_mpi.values())[-2:]] )


# MPI Figure: time vs process
figFilename = "mpi_process-time"
(fig5, ax5) = plt.subplots(1, 1, facecolor = 'w', figsize=(13,7))

fig5.suptitle(f"Tempo de Execução: Média e Desvio Padrão ({numExecutions} execuções)")
def plot5():
    for size,fmtString in zip(x_arraySizes_mpi, [".--r", "*--y", "2--g", "s--c", "p--b", "o--m"]):
        ax5.errorbar(x_numProcess_mpi, y_timeMean_byArraySizes_mpi[size], yerr=y_timeStd_byArraySizes_mpi[size], fmt=fmtString, lw=2.7, label=f"Tamanho do Array: {size}", capsize=8)


    ax5.set_ylabel("Tempo (milissegundos)")
    ax5.set_yticks(y5_tickLabels_lin, labels=np.round(y5_tickLabels_lin, decimals=4))

    ax5.set_xlabel("Processos")
    ax5.set_xticks(x_numProcess_mpi)


    ax5.grid()
    ax5.legend()
plot5()
fig5.tight_layout()
fig5.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")




# Separating tick labels that will be displayed linearly and logarithmically
y6_tickLabels_log = np.concatenate( [arr for arr in list(y_speedup_byArraySizes_mpi.values())[1:]] )
y6_tickLabels_lin = np.concatenate( [arr for arr in y_performance_byArraySizes_mpi.values()] )


# OMP Figure: speedup
figFilename = "mpi_speedup"
(fig6, (lg6, ax6)) = plt.subplots(1, 2, facecolor = 'w', figsize=(15,7))

fig6.suptitle(f"Speedup e Desempenho médio ({numExecutions} execuções)")
def plot6():
    lg6.set_title("Speedup")
    ax6.set_title("Desempenho")

    lg6.plot(x_numProcess_mpi, [1 for ugh in x_numProcess_mpi], ":k", lw=2.7)
    ax6.plot(x_numProcess_mpi, [100 for ugh in x_numProcess_mpi], ":k", lw=2.7)

    for size,fmtString in zip(x_arraySizes_mpi, [".--r", "*--y", "2--g", "s--c", "p--b", "o--m"]):
        lg6.plot(x_numProcess_mpi, y_speedup_byArraySizes_mpi[size], fmtString, lw=2, label=f"Tamanho do Array: {size}")
        ax6.plot(x_numProcess_mpi, y_performance_byArraySizes_mpi[size], fmtString, lw=2, label=f"Tamanho do Array: {size}")


    lg6.set_xlabel("Processos")
    lg6.set_xticks(x_numProcess_mpi, labels=x_numProcess_mpi)

    lg6.set_yticks(y6_tickLabels_log, labels=np.round(y6_tickLabels_log, decimals=3))

    ax6.set_xlabel("Processos")
    ax6.set_xticks(x_numProcess_mpi, labels=x_numProcess_mpi)

    ax6.set_yticks(y6_tickLabels_lin, labels=[f"{prctg} %" for prctg in np.round(y6_tickLabels_lin, decimals=2)])


    lg6.grid()
    ax6.grid()

    lg6.legend()
    ax6.legend()
plot6()
fig6.tight_layout()
fig6.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")




# Comparison Figure: time vs size
figFilename = "compare_size-time"
( fig7, ((lg1, ax1), (lg4, ax4)) ) = plt.subplots(2, 2, facecolor = 'w', figsize=(13,10))
fig7.suptitle(f"Comparação de Tempo de Execução: OpenMP (acima) e OpenMPI (abaixo)")
plot1()
plot4()
fig7.tight_layout()
fig7.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")




# Comparison Figure: time vs process
figFilename = "compare_process-time"
( fig8, (ax2, ax5) ) = plt.subplots(2, 1, facecolor = 'w', figsize=(11,9))
fig8.suptitle(f"Comparação de Tempo de Execução: OpenMP (acima) e OpenMPI (abaixo)")
plot2()
plot5()
fig8.tight_layout()
fig8.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")




# Comparison Figure: time vs size
figFilename = "compare_speedup"
( fig9, ((lg3, ax3), (lg6, ax6)) ) = plt.subplots(2, 2, facecolor = 'w', figsize=(13,13))
fig9.suptitle(f"Comparação de Speedup e Desempenho médio: OpenMP (acima) e OpenMPI (abaixo)")
plot3()
plot6()
fig9.tight_layout()
fig9.savefig(f"{os.path.join(picSaveDir, figFilename)}.png")