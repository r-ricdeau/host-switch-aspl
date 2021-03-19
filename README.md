# host-switch-ASPL
Computation of host-to-host average shortest path length (h-ASPL) and host-to-host diameter (h-diameter) for host-switch graphs

## Graph file
* The first line describes the following three parameters: n (the number of hosts), m (the number of switches), and r (the number ports of each switch, namely radix).
* The hosts must be integers 0, 1, 2, ..., n - 2, n - 1. The switches must be integers n, n + 1, n + 2, ..., n + m - 2, n + m - 1.
* From the second line, an edge list format follows the first line.

### Example (ex.txt)
```
4 2 4
0 4
1 4
2 4
3 5
4 5
```

## Compilation
```
$ make
```

## Execution
```
$ ./h-aspl graph_file
h-ASPL
h-diameter
```
### Example
```
$ ./h-aspl ex.txt
2.500000
3
```
