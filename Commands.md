
# Outros Comandos

~~Não está organizado.~~

---

Rodar todos os testes e gerar os gráficos:

```shell
gcc     seqms.c     -o seqms    -fopenmp
gcc     parms.c     -o parms    -fopenmp
mpicc   mpims.c     -o mpims
bash run-sequential-tests.sh  10
bash run-parallel-tests.sh    10
bash run-distributed-tests.sh 10
mkdir -p Pictures
python3 plot-graphs.py
```

Ou (equivalente):

```shell
bash run-all.sh 10
```

Gerar resultados com versão alternativa do `seqms.c`:

```shell
gcc     seqms.c     -o seqms    -fopenmp
gcc Alternative/parms_explicit.c  -o parms  -fopenmp
mpicc   mpims.c     -o mpims
bash run-sequential-tests.sh  10
bash run-parallel-tests.sh    10
bash run-distributed-tests.sh 10
mkdir -p Pictures
python3 plot-graphs.py
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