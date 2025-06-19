# PathJoin

This repository contains the C implementation of Pathjoin, designed to identify all cycles of a specified length in a simple graph. The algorithm supports both directed and undirected graphs and can be configured to use various path-join strategies to construct the target cycles.

## Features

- Supports graphs in [Pajek `.net`](https://gephi.org/users/supported-graph-formats/pajek-net-format/) format
- Enumerates cycles of a user-specified length
- Supports 2-join, 3-join, and 4-join configurations
- Efficient implementation using dynamic memory management
- Optional two-core preprocessing for optimization
- Customizable output and verbosity

## Requirements

- C compiler (e.g., `gcc`)
- `make` utility
- [`uthash.h`](https://troydhanson.github.io/uthash/) v2.3.0 (included in the repository)

## Compilation

To compile the program, run:

```
make
```

This will generate an executable named `main`.

To remove all compiled files:

```
make clean
```

## Usage

```
./main <filepath> <cycle length> [options]
```

### Required Parameters

- `<filepath>`: Path to the input graph file in Pajek `.net` format.
- `<cycle length>`: Integer specifying the target cycle size.

### Optional Parameters

- `-d <true|false>`: Flag indicating if the graph is directed. Default: `false`.
- `-c <int1> <int2> [<int3> <int4>]`: Join configuration. A sequence of integers representing the lengths of paths to be joined. Must sum to the cycle length. Supports 2-join, 3-join, or 4-join.
- `-o [<filename>]`: Output file name. If specified, the program writes results (cycle count and list of cycles) to the file.
- `-v`: Enables verbose output to standard output.
- `-twocore <true|false>`: Enables/disables two-core preprocessing. Default: `true`.

## Example

```
./main graph.net 6 -d false -c 3 3 -o output.txt -v -twocore true
```

This command will:
- Read the undirected graph from `graph.net`
- Look for cycles of length 6 using a 3+3 path join configuration
- Write the list of found cycles and their count to `output.txt`
- Print verbose execution details
- Use the two-core optimization

## License

This project is released under the MIT License. See `LICENSE` for details.

## Acknowledgements

- [`uthash.h`](https://troydhanson.github.io/uthash/) — a lightweight, efficient hash table implementation for C.
