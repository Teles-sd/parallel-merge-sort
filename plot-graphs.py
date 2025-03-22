import numpy as np
import pandas as pd
import matplotlib.pyplot as plt

import os

# Directory of the script being run
scriptDir = os.path.dirname(os.path.abspath(__file__))
picSaveDir = os.path.join(scriptDir, "Pictures")
# print(picSaveDir)

sequentialCsvFilename = "sequential-tests.csv"
parallelCsvFilename = "parallel-tests.csv"

sequentialCsvPath = os.path.join(scriptDir, sequentialCsvFilename)
parallelCsvPath = os.path.join(scriptDir, parallelCsvFilename)

if os.path.isfile(sequentialCsvPath):
    # print("Yay 1")
    dfSeq = pd.read_csv(sequentialCsvPath, index_col='Run')

    # dfSeq.info()
    # print(dfSeq)
    # print(dfSeq.head())

    # print(dfSeq["Array_size"].dtype)
    arraySizes = dfSeq["Array_size"].unique()
    timeMean = np.zeros(len(arraySizes), dtype=dfSeq["Time_ms"].dtype)
    timeStd  = np.zeros(len(arraySizes), dtype=dfSeq["Time_ms"].dtype)
    # print(arraySizes)
    # print(timeMean)

    for i in range(len(arraySizes)):
        # print(i)
        # print(arraySizes[i])
        # print(dfSeq[dfSeq["Array_size"] == arraySizes[i]]["Time_ms"].mean())
        timeMean[i] = dfSeq[dfSeq["Array_size"] == arraySizes[i]]["Time_ms"].mean()
        timeStd[i]  = dfSeq[dfSeq["Array_size"] == arraySizes[i]]["Time_ms"].std()

    # print(arraySizes.dtype)
    # print(pd.DataFrame([arraySizes,timeMean,timeStd]))

if os.path.isfile(parallelCsvFilename):
    # print("Yay 2")
    dfPar = pd.read_csv(parallelCsvFilename, index_col='Run')

    numThreads = dfPar["Num_threads"].unique()

    arraySizesParallel = dfPar["Array_size"].unique()
    timeMeanParallel = {t:np.zeros(len(arraySizesParallel), dtype=dfPar["Time_ms"].dtype)    for t in numThreads}
    timeStdParallel  = {t:np.zeros(len(arraySizesParallel), dtype=dfPar["Time_ms"].dtype)    for t in numThreads}

    for n in numThreads:

        for i in range(len(arraySizesParallel)):

            timeMeanParallel[n][i] = dfPar[ (dfPar["Num_threads"] == n) & (dfPar["Array_size"] == arraySizesParallel[i]) ]["Time_ms"].mean()
            timeStdParallel[n][i]  = dfPar[ (dfPar["Num_threads"] == n) & (dfPar["Array_size"] == arraySizesParallel[i]) ]["Time_ms"].std()
    
    # print(timeMeanParallel)
    # print(timeStdParallel)


# print(pd.DataFrame([arraySizes,timeMean,timeStd]))

# timeMeanAll = timeMean[:]
timeMeanAll_lin = timeMean[3:]
timeMeanAll_log = timeMean[:3]

# for array in timeMeanParallel.values():
# for array in list(timeMeanParallel.values())[1:]:         # not including first (2 threads), cus too close
#     timeMeanAll = np.append(timeMeanAll, array)
for array in list(timeMeanParallel.values()):
    timeMeanAll_lin = np.append(timeMeanAll_lin, array[3:])
    timeMeanAll_log = np.append(timeMeanAll_log, array[:3])

# timeMeanAll = sorted(timeMeanAll)

# with plt.style.context('default'):

# (fig, (ax, lg)) = plt.subplots(1, 2, facecolor = 'w', figsize=(25,10))
# (fig, (lg, ax)) = plt.subplots(1, 2, facecolor = 'w', figsize=(25,10))
(fig, (lg, ax)) = plt.subplots(1, 2, facecolor = 'w', figsize=(15,7))

fig.suptitle("Média e Desvio Padrão do tempo de execução")

ax.set_title("Tempo em escala linear")
lg.set_title("Tempo em escala logarítmica")

# ax.plot(arraySizes, timeMean, ".--k",label="1 proc")
# ax.errorbar(arraySizes, timeMean, yerr=timeStd, fmt=".--k", label="1 thread", capsize=6)
# lg.errorbar(arraySizes, timeMean, yerr=timeStd, fmt=".--k", label="1 thread", capsize=6)
ax.errorbar(arraySizes[3:], timeMean[3:], yerr=timeStd[3:], fmt=".--k", label="1 thread", capsize=6)
lg.errorbar(arraySizes[:3], timeMean[:3], yerr=timeStd[:3], fmt=".--k", label="1 thread", capsize=6)

for n,f in zip(numThreads, ["*--r","2--y","s--g","p--b","o--m"]):
    ax.errorbar(arraySizesParallel[3:], timeMeanParallel[n][3:], yerr=timeStdParallel[n][3:], fmt=f, label=f"{n} threads", capsize=6)
    lg.errorbar(arraySizesParallel[:3], timeMeanParallel[n][:3], yerr=timeStdParallel[n][:3], fmt=f, label=f"{n} threads", capsize=6)

ax.set_xscale('log')
lg.set_xscale('log')
ax.set_xticks(arraySizes[3:], labels=arraySizes[3:])
lg.set_xticks(arraySizes[:3], labels=arraySizes[:3])
ax.tick_params(axis="x", rotation=30)
lg.tick_params(axis="x", rotation=30)
# ax.set_xlabel("Tamanho do Array (escala logarítmica)")
# lg.set_xlabel("Tamanho do Array (escala logarítmica)")
ax.set_xlabel("Tamanho do Array")
lg.set_xlabel("Tamanho do Array")

lg.set_yscale('log')
# ax.set_yticks(timeMeanAll, labels=np.round(timeMeanAll, decimals=4))
# lg.set_yticks(timeMeanAll, labels=np.round(timeMeanAll, decimals=4))
ax.set_yticks(timeMeanAll_lin, labels=np.round(timeMeanAll_lin, decimals=4))
lg.set_yticks(timeMeanAll_log, labels=np.round(timeMeanAll_log, decimals=4))
# ax.set_yticks(timeMean, labels=np.round(timeMean, decimals=4))
# ax.set_yticks(timeMeanAll, labels=np.round(timeMeanAll, decimals=4))
# ax.set_ylabel("Tempo (milissegundos)")
lg.set_ylabel("Tempo (milissegundos)")
# lg.set_ylabel("Tempo (milissegundos, escala logarítmica)")

ax.grid()
lg.grid()

ax.legend()
lg.legend()

fig.tight_layout()

figName = "size-time"
fig.savefig(f"{os.path.join(picSaveDir, figName)}.png")
# fig.savefig(f"{os.path.join(picSaveDir, figName)}.jpeg")


# ax.set_ylabel("Tempo (milissegundos, escala logarítmica)")

# figName = "size-time-log"
# fig.savefig(f"{os.path.join(picSaveDir, figName)}.png")
# fig.savefig(f"{os.path.join(picSaveDir, figName)}.jpeg")


# fig.savefig(f"{os.path.join(picSaveDir, figName)}.png", dpi=300)
# fig.savefig(f"{os.path.join(picSaveDir, figName)}.jpeg", dpi=300)
# plt.show()