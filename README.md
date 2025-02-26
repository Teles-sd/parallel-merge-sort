
# Implementação de Merge-Sort Paralelo

## Requisitos

- [x] Implementação sequencial ([seqms.c](./seqms.c)).
- [ ] Implementação paralela ([parms.c](./parms.c)).
- [ ] Testar métricas de desempenho com 1, 2, 4, 8 e 12 _threads_ (se disponíveis no sistema).
  - _Speedup_, eficiência e desempenho.
  - Média de 5+ execuções para cada medida.

## Compile

Compilar versão sequencial:

```shell
gcc  seqms.c  -o seqms
```

Testar versão sequencial:
```shell
./seqms
```
```shell
./seqms 100
```


Compilar versão paralela:

```shell
gcc  parms.c  -o parms  -fopenmp
```

## OpenMP

Version: `_OPENMP = '201511'`  
Nov 2015: OpenMP 4.5