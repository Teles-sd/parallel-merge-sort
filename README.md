
# Implementação de Merge-Sort Paralelo

## Requisitos da tarefa

- [x] Implementação sequencial ([seqms.c](./seqms.c)).
- [x] Implementação paralela com OpenMP ([parms.c](./parms.c)).
- [x] Implementação distribuída com OpenMPI ([mpims.c](./mpims.c)).
- [x] Testar métricas de desempenho com 2, 4, 8 _threads_ (se disponíveis no sistema).
  - [x] _Speedup_, eficiência e desempenho.
  - [x] Média de 5+ execuções para cada medida.
- [x] Validar os resultados (verificar automaticamente se ordenado).


## Testes

Rodar todos os testes e gerar os gráficos:

```shell
bash run-all.sh 10
```

Para ver outros comandos: [Commands.md](./Commands.md).


## Resultados

- Comparação entre a implementação com OpenMP (gráficos acima) e OpenMPI (gráficos abaixo) do **tempo de execução** (média e desvio padrão) em função do **tamanho do array** ordenado:

<img src="./Pictures/compare_size-time.png" width="80%"/>

<br/>

- Comparação entre a implementação com OpenMP (gráficos acima) e OpenMPI (gráficos abaixo) do **tempo de execução** (média e desvio padrão) em função do **número de threads/processos**:

<img src="./Pictures/compare_process-time.png" width="80%"/>

<br/>

- Comparação entre a implementação com OpenMP (gráficos acima) e OpenMPI (gráficos abaixo) do **Speedup** e **Desempenho** médio em função do **número de threads/processos**:

<img src="./Pictures/compare_speedup.png" width="80%"/>

<br/>


## Ferramentas Utilizadas

[<img src="https://upload.wikimedia.org/wikipedia/commons/1/18/C_Programming_Language.svg" 
      alt="C Programming Language"
      height="40"/>][c]
[<img src="https://upload.wikimedia.org/wikipedia/commons/4/40/OpenMP_logo.svg" 
      alt="OpenMP"
      height="40"/>][omp]
[<img src="https://www.open-mpi.org/images/open-mpi-logo.png" 
      alt="OpenMPI"
      width="40"
      height="40"/>][mpi]
[<img src="https://www.svgrepo.com/show/353478/bash-icon.svg" 
      alt="bash"
      width="40"
      height="40"/>][bash]
[<img src="https://www.vectorlogo.zone/logos/python/python-icon.svg" 
      alt="python"
      width="40"
      height="40"/>][python]

[c]:https://www.c-language.org/
[omp]:https://www.openmp.org/
[mpi]:https://www.open-mpi.org/
[bash]:https://www.gnu.org/software/bash/
[python]:https://www.python.org/

- Implementação do algorítimo:
  - [C][c], versão: `gcc (GCC) 14.2.1 20250207`
  - [OpenMP][omp], versão: `_OPENMP = '201511'`; Nov 2015: OpenMP 4.5
  - [OpenMPI][mpi], versão:
    - `mpicc: Open MPI 5.0.7 (Language: C)`
    - `mpirun (Open MPI) 5.0.7`
- Script para rodar os testes multiplas vezes:
  - [Bash][bash], versão: `GNU bash, version 5.2.37(1)-release (x86_64-pc-linux-gnu)`
- Gráficos: 
  - [Python][python], versão: `Python 3.13.2`
    - Numpy
    - Pandas
    - Matplotlib

