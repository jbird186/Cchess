# Cchess

A straightforward command-line chess engine written in C.

## Compiling

Compiled binaries are created in `./build/bin/`.

### Make

* `make`: Compile the chess engine in release mode.
* `make debug`: Compile with debug symbols enabled for easier debugging and development.
* `make clean`: Remove compiled binaries and object files.

### CMake

1. Build:
    ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
    cmake --build build --config Release
    ```
2. Debug build:
    ```bash
    cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
    cmake --build build --config Debug
    ```
3. Clean:
    ```bash
    rm -rf build
    ```

## Commands

Here is a list of the available commands:

* `help`: Display a list of available commands.
* `exit`: Exit the program.
* `reset`: Reset the board and clear history.
* `back [n]`: Undo previous `n` moves (default 1).
* `lock`: Lock board orientation to current player's perspective.
* `unlock`: Unlock board orientation to switch between each player's perspective.
* `history`: Display move history.
* `list`: List all legal moves for current position.
* `perft <depth>`: Count all possible positions up to `depth`, starting from the current position.
* `play`: Computer makes the best move for the current player.
* `auto`: Enable automatic play for the current player.
* `<move>`: Enter a legal move in algebraic coordinates (e.g., `e2e4`, `g7g8q`). Promotion suffixes: `n`=Knight, `b`=Bishop, `r`=Rook, `q`=Queen.

Some commands can be preloaded when launching the engine. Commands are executed in order, allowing you to start the engine with predefined behavior without interactive input. For example:

```bash
./build/bin/chess auto lock
```

* `auto`: Sets the current player (white at start) to be controlled by the engine.
* `lock`: Locks the board orientation to the perspective of the current player (now black).

## Config

Configurable options are defined in [`./src/config.h`](./src/config.h). Any changes to this file require the project to be recompiled for the changes to take effect.

## License

This project is open source and licensed under the MIT license. see the [`LICENSE`](LICENSE) file for more details.