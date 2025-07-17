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
g++ -std=c++17 -Iinclude -I/opt/homebrew/opt/openssl@3/include src/*.cpp -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto -lsqlite3 -o bin/PasswordManager
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


## Running tests

This project uses Catch2 for unit testing.
You can build the tests manually with g++.

Example: Build and run the UserManager test

```bash
g++ -std=c++17 \
  tests/test_user_manager.cpp \
  src/UserManager.cpp \
  src/DatabaseManager.cpp \
  src/StatementWrapper.cpp \
  src/Logger.cpp \
  src/PasswordHasher.cpp \
  -Iinclude \
  -I/opt/homebrew/opt/openssl@3/include \
  -L/opt/homebrew/opt/openssl@3/lib \
  -lssl -lcrypto -lsqlite3 \
  -o bin/test_user_manager
```

Then execute the test binary:

```bash
./bin/test_user_manager
```


Example: Build and run the DatabaseManager test

```bash
g++ -std=c++17 \
  tests/test_database_manager.cpp \
  src/DatabaseManager.cpp \
  src/Logger.cpp \
  -Iinclude \
  -lsqlite3 \
  -o bin/test_database_manager
  ```

Then execute the test binary:

```bash
./bin/test_database_manager
```


Example: Build and run the AccountManager test

```bash
g++ -std=c++17 \
  tests/test_account_manager.cpp \
  src/UserManager.cpp \
  src/AccountManager.cpp \
  src/DatabaseManager.cpp \
  src/StatementWrapper.cpp \
  src/Logger.cpp \
  src/PasswordHasher.cpp \
  -Iinclude \
  -I/opt/homebrew/opt/openssl@3/include \
  -L/opt/homebrew/opt/openssl@3/lib \
  -lssl -lcrypto -lsqlite3 \
  -o bin/test_account_manager
```

Then execute the test binary:

```bash
./bin/test_account_manager
```