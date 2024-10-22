# extism-dbg

Debugging for [Extism](https://github.com/extism/extism) plugins

![extism-dbg](extism-dbg.png)

## Installation

```bash
$ make && sudo make install
```

This will install `extism-dbg` and `extism-lldb`:
- `extism-dbg` is a minimal Extism host used to wrap the plugin.
- `extism-lldb` is a shim to use LLDB to debug Extism plugins called from `extism-dbg` 

## Usage

The following command will start LLDB with a breakpoint set on the specified plugin function, 
just type `run` at the LLDB prompt to start executing the plugin:

```bash
$ extism-lldb count-vowels.wasm count_vowels --input @input.txt
```

Arguments:
1. WebAssembly module or Manifest file
2. Function to call

See the output of `extism-lldb --help` for a list of options.

- The `--source` parameter can be used to specify the search path(s) for the original source files 
- If the `--input` parameter starts with `@` then the input will be loaded from a file otherwise it will 
  be passed as-is. 
- `--lldb` can be used to specify the LLDB executable
- Any arguments after the function name will be passed to LLDB directly
