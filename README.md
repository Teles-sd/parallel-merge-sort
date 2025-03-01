
# Implementação de Merge-Sort Paralelo

## Requisitos

- [x] Implementação sequencial ([seqms.c](./seqms.c)).
- [ ] Implementação paralela ([parms.c](./parms.c)).
- [ ] Testar métricas de desempenho com 1, 2, 4, 8 e 12 _threads_ (se disponíveis no sistema).
  - _Speedup_, eficiência e desempenho.
  - Média de 5+ execuções para cada medida.

## Versão sequencial

Compilar (OpenMP necessário pelas funções de temporização):

```shell
gcc  seqms.c  -o seqms  -fopenmp
```

Testar:
```shell
./seqms
```
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
```shell
sh run-sequential-tests.sh 
```

## Versão paralela

Compilar:

```shell
gcc  parms.c  -o parms  -fopenmp
```

## OpenMP

Version: `_OPENMP = '201511'`  
Nov 2015: OpenMP 4.5