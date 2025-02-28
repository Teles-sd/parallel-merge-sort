
# Anotações para o Artigo

---

On the sequential code implementation, for an array of size $n$, the "Max depth reached" ($\textit{maxDepth}$) represents the number of subdivisions needed until it reaches an subarray of size $1$. Since it aways divides and array/subarray in half, it will be the $\log_2$ of the array size, rounded up:

$$
\textit{maxDepth} = \lceil \log_2{n} \rceil
$$

It will then read through each element **once** while sorting them, and thus this give the algorithm complexity of:

$$
\textit{O}(n \log n)
$$