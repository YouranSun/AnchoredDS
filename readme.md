## PASTA

This repository contains C++ codes for the paper:

> Efficient Anchored Densest Subgraph Discovery: Improved Time Complexity and Practical Performance

### Introduction

We study the problem of anchored densest subgraph (ADS) problem aims to detect a densest subgraph that is anchored around user-defined seed vertices. We first propose a novel approximation algorithm with improved time complexity, achieving the same accuracy as existing methods while requiring significantly fewer iterations. To further enhance practical performance, we introduce a graph reduction technique that localizes the ADS search to a much smaller subgraph, while still providing non-trivial theoretical guarantees. Building on this approximation, we also develop an efficient exact algorithm. We have performed an extensive empirical evaluation of our approaches on 12 real large datasets. The results show that our proposed algorithms are up to four orders of magnitude faster than the state-of-the-art.

### Environment

We implement all the algorithms under the following environment:

* Language: C++ 17 
* Hardware: Intel(R) Xeon(R) Gold 6338 CPU @ 2.00GHz, 512GB of memory
* OS: Ubuntu 20.04.1

### Datasets

We used the following 10 real-world datasets:

| Dataset | Category | $\lvert V\rvert$ | $\lvert E\rvert$ |
|-|-|-|-|
| OpenFlights (OF) | Infrastructure | 2,939 | 15,677 |
| Ego-Facebook (EF) | Social | 4,039 | 88,234 |
| Email (EM) | Communication | 265,009 | 364,481 |
| Com-Amazon (CA) | Comments | 334,863 | 925,872 |
| DBLP (DP) | Co-author | 317,080 | 1,049,866 |
| Web-Google (WG) | Web | 875,713 | 5,105,039 |
| Youtube (YT) | Friendship | 3,223,585 | 9,375,374 |
| Cit-Patents (CP) | Citation | 3,774,768 | 16,518,948 | 
| LiveJournal (LJ) | Social | 4,847,571 | 68,993,773 |
| UK-2002 (UK) | Web | 18,520,486 | 298,113,762 | 
| IT-2004 (IT) | Hyperlink | 41,291,594 | 1,027,474,947 |
| SK-2005 (SK) | Web | 50,636,154 | 1,949,412,601 |


### Run the codes

#### Compilation

```sh
cmake -S ./
make
```

#### Execution

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

| Parameter | Meaning | Value |
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

##### Example

graph.txt
```
7 9
0 2
0 5
1 2
1 3
1 4
2 3
3 4
4 5
5 6
```

A.txt
```
7 1
3
```

R.txt
```
7 3
2 3 4
```
