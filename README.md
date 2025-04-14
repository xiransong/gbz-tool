# gbz-tool

A command-line utility for working with GBZ files, providing functionalities to inspect, convert, and query graph data.

## Installation

Clone the repository and install dependencies:

```bash
git clone https://github.com/xiransong/gbz-tool.git
cd gbz-tool
bash install_dependency.sh
```

## Compilation

Compile the tool using:

```bash
make
```

## Features

`gbz-tool` supports the following commands:

- `info <input.gbz>`: Displays information about a GBZ file.
- `convert <input.gfa> <output.gbz>`: Converts a GFA file to GBZ format.
- `find <input.gbz> <node_id>`: Retrieves the node sequence for a given node ID.
- `find-inter <input.gbz>`: Enters interactive mode to query node sequences.
- `find-batch <input.gbz> <node_ids.txt> <node_info.json>`: Batch processes node IDs and outputs results to a JSON file.

## Usage

Run the tool with:

```bash
./gbztool <command> [arguments]
```

### Example

Interactive mode example:

```bash
xsong@maotai:gbz-tool$ ./gbztool find-inter ../data/hprc-v1.1-mc-grch38.gbz 
Loaded ../data/hprc-v1.1-mc-grch38.gbz. Enter node ID to query (or 'exit' to quit):
> 2578930
Node 2578930: [TCAAG...]
```
