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
- `find-batch <input.gbz> <node_ids.txt> <node_info.json>`: Batch processes node IDs and outputs results to a JSON file. Each line in `node_ids.txt` is a node ID. 

## Usage

Run the tool with:

```bash
./gbztool <command> [arguments]
```

### Example

Interactive mode example:

```bash
xsong@guinness:/scratch/xsong/project/gbz-tool$ ./gbztool find-inter ../data/hprc-v1.1-mc-grch38.gbz
Loaded ../data/hprc-v1.1-mc-grch38.gbz. Enter node ID to query (or 'exit' to quit):
> 924
924: {
    "haplotype_count": 7,
    "haplotypes": [
    {
      "chromosome": "HG00621",
      "haplotype_id": 206,
      "haplotype_number": 2,
      "path_type": "Haplotype",
      "region": "JAHBCC010000130.1",
      "thread_context": [
        {
          "id": 921,
          "is_target": false
        },]

        ...
    
    "in_edges": [
    {
      "from": 922,
      "from_orientation": "-",
      "to": 924,
      "to_orientation": "+"
    }
    ],
    "node_id": 924,
    "out_edges": [
    {
        "from": 924,
        "from_orientation": "+",
        "to": 925,
        "to_orientation": "-"
    }
    ],
    "sequence": "C",
    "sequence_length": 1
    }
```
