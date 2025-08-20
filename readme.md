## Efficient Anchored Densest Subgraph Discovery: Improved Time Complexity and Practical Performance

#### Compile

```sh
cmake -S ./
make
```

#### Run

##### PASTA
```sh
./bin/AnchoredDS -g <graph_file> -r <R_file> -a <A_file> -d FISTA -c -T 1024
```

##### PASTA-exact
```sh
./bin/AnchoredDS -g <graph_file> -r <R_file> -a <A_file> -d FISTA -c -T 1024 -e -Texact 100
```

##### FDP
```sh
./bin/AnchoredDS -g <graph_file> -r <R_file> -a <A_file> -d FDP -T 1024
```

##### FW
```sh
./bin/AnchoredDS -g <graph_file> -r <R_file> -a <A_file> -d FW -T 1024
```

##### Parameters

| Parameter | Meaning | Value
|-|-|-|
| `-g` | File path of $G$ | Path |
| `-r` | File path of $R$ | Path |
| `-a` | File path of $A$ | Path |
| `-d` | Method name | {`FISTA`, `FW`, `FDP`} |
| `-c` | Use graph reduction technique | N/A |
| `-T` | Max number of iterations | Integer |
| `-e` | Exact | N/A |
| `-Texact` | Number of iterations per validation | Integer |

#### Data format

All the vertices are $0$-indexed.

##### Graph $G$

$n, m$ are the number of vertices and edges in the graph.

```
n m
u_1 v_1
u_2 v_2
...
u_m v_m
```

##### Vertex set $R$ and $A$

$l$ is the size of the vertex set.

```
n l
u_1 u_2 ... u_l
```