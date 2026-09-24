# FlatDB

FlatDB is a small, single-process, flat-file JSON database engine with a
SQL-like command language. There's no relational engine, indexing, or WAL —
each database is a directory, each table is a subdirectory holding a
`Schema.json` and a `Data.json`, and every insert/update reads the whole
table into memory, mutates it, and rewrites the file atomically.

## Architecture

| Layer | Files | Responsibility |
| --- | --- | --- |
| Utils | `include/Utils.h`, `src/Utils.cpp` | Global constants, CLI help text, ASCII table printing, schema type validation |
| Error | `include/Error.h` | `ERROR_CODE` enum + message lookup, thrown/caught as the exception type throughout |
| Parser | `include/Parser.h`, `src/Parser.cpp` | Tokenizes and dispatches the SQL-like command language; owns "current database" state |
| JSONDB | `include/JSONDB.h`, `src/JSONDB.cpp` | Storage engine: durable JSON file read/write, table create/insert/read primitives |

### On-disk layout

```plaintext
<DATABASE_DIRECTORY>/
  <db_name>/
    <table_name>/
      Schema.json   # {"colName": "type", ...}  types: int, float, string, char, array
      Data.json     # [ {record}, {record}, ... ]
```

## Storage engine (`JSON_DB`)

- **Durable writes**: every write goes to a `.tmp` sibling file, is flushed
  and `fsync`'d, then atomically renamed over the target, followed by an
  `fsync` of the parent directory — a crash-safe write pattern (POSIX via
  `fsync`, Windows via `FlushFileBuffers`).
- **No indexing**: reads and writes are O(n) over the full `Data.json`
  array — fine for small tables, not designed for scale.
- **Concurrency**: a per-table `mutex` guards file I/O within one process;
  there is no cross-process locking.
- **Path safety**: `Resolve_DB_Path` in the parser rejects `..` segments and
  verifies the canonicalized path stays inside `DATABASE_DIRECTORY`.

## Command language (`Parse`)

Supported verbs: `CREATE`, `USE`, `TRUNCATE`, `DROP`, `INSERT`, `SELECT`,
`UPDATE`, `HELP`, `EXIT`.

```sql
CREATE DATABASE <name>
CREATE TABLE <name> FROM <schema.json>
USE DATABASE <name>
INSERT INTO TABLE <name> VALUES key=val key2=val2 ...
SELECT * FROM <table> [WHERE key <op> val]
UPDATE <table> SET key=val WHERE key <op> val
TRUNCATE DATABASE <name> | TRUNCATE TABLE <name>
DROP DATABASE <name> | DROP TABLE <name>
```

Relational operators: `=`, `!=`, `<`, `>`, `<=`, `>=`.

**Known limitations:**

- Only a single `WHERE` condition is supported end-to-end, even though the
  built-in help text advertises `AND`/`OR` combinators.
- `WHERE` comparisons are done as string comparisons (via JSON `.dump()`),
  so numeric ordering (`<`, `>`) on numeric fields is lexicographic, not
  numeric.

## Building

Requirements: CMake ≥ 3.15, a C++20 compiler, network access on first
configure (to fetch the JSON dependency).

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

This produces a single `FlatDB` executable built from
`src/JSONDB.cpp`, `src/Parser.cpp`, `src/Utils.cpp`, and `src/Main.cpp`.

### Dependency

- [nlohmann/json](https://github.com/nlohmann/json) v3.11.3, fetched
  automatically via CMake `FetchContent`.

### Configuration required before building

`DATABASE_DIRECTORY` in [include/Utils.h](include/Utils.h) is a hardcoded,
machine-specific absolute path where databases are stored on disk. Update it
to a valid path on your machine before building.

## Testing

There is no test framework wired into CMake (no CTest/GoogleTest/Catch2).
`src/Main.cpp` is a work in progress and not covered by this document.

## Error handling

Errors are represented by the `ERROR_CODE` enum (`include/Error.h`) and
thrown as values rather than `std::exception` subclasses. Each code has a
human-readable message (including expected command syntax) via
`GET_ERROR_MESSAGE()`, covering file, directory, database, table, and
per-command syntax errors.
