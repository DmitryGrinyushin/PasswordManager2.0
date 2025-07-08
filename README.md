# PasswordManager2.0

A simple C++ password manager with SQLite storage and environment-based configuration.


## Environment variables

These variables configure the database and log file locations:

```bash
export PASSWORD_MANAGER_DB_PATH="$HOME/NewProject/PassManager2/data/passdb.sqlite"
export PASSWORD_MANAGER_LOG_PATH="$HOME/NewProject/PassManager2/log.txt"
```


## Build

A C++17-compatible compiler is required:

```bash
g++ -std=c++17 -Iinclude src/*.cpp -lsqlite3 -o bin/PasswordManager
```


## Features:

- SQLite database initialization
- Logging with configurable path
- Environment variable support
- User management (in progress)


## Usage examples:

```bash
./bin/PasswordManager
```