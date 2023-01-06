# extism-dbg

A debugger for [Extism](https://github.com/extism/extism) plugins using LLDB

## Installation

```bash
$ make build && sudo make install
```

## Usage

```bash
$ extism-lldb code.wasm count_vowels @input.txt
```
Arguments:
1. WebAssembly module or Manifest file
2. Function to call
3. Input
 
If the `input` argument starts with `@` then the input will be loaded from a file, otherwise it will be passed as-is.
