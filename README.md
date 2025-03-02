
# Implementação de Merge-Sort Paralelo

## Requisitos da tarefa

- [x] Implementação sequencial ([seqms.c](./seqms.c)).
- [x] Implementação paralela ([parms.c](./parms.c)).
- [ ] Testar métricas de desempenho com 1, 2, 4, 8 e 12 _threads_ (se disponíveis no sistema).
  - [ ] _Speedup_, eficiência e desempenho.
  - [x] Média de 5+ execuções para cada medida.

## Uso

Rodar todos os testes e gerar os gráficos:

```shell
gcc  seqms.c  -o seqms  -fopenmp
gcc  parms.c  -o parms  -fopenmp
sh run-sequential-tests.sh  10
sh run-parallel-tests.sh    10
mkdir -p Pictures
py plot-graphs.py
```

## Versão sequencial

Compilar (OpenMP necessário pelas funções de temporização):

```shell
gcc  seqms.c  -o seqms  -fopenmp
```

Testar:

```shell
./seqms 100
```
```shell
./seqms -h
```

Rodar múltiplos testes usando diferentes tamanhos de vetores e salvando os resultados em [`sequential-tests.csv`](./sequential-tests.csv):

```shell
sh run-sequential-tests.sh 5
```

## Versão paralela

Compilar:

```shell
gcc  parms.c  -o parms  -fopenmp
```

Testar:

```shell
./seqms 100
```
```shell
./seqms -h
```

Rodar múltiplos testes usando diferentes tamanhos de vetores e salvando os resultados em [`parallel-tests.csv`](./parallel-tests.csv):

```shell
sh run-parallel-tests.sh 5
```

## OpenMP

Version: `_OPENMP = '201511'`  
Nov 2015: OpenMP 4.5
