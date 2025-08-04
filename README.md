# PasswordManager2.0

A simple C++ password manager with SQLite storage, AES-256-GCM encryption (via OpenSSL), and PBKDF2 password hashing.
Configured via environment variables and managed through a CLI interface.
Core functionality is covered by unit tests using Catch2.


## Features

- AES-256-GCM encryption for account data (via OpenSSL)
- Password hashing using PBKDF2 with 100,000 iterations
- SQLite database with automatic initialization
- Logging with configurable log path
- Environment-based configuration
- User management: registration, login, deletion
- Account management: create, update, delete, list
- Catch2 test coverage for core components


## Environment variables

These variables configure the database and log file locations:

```bash
export PASSWORD_MANAGER_DB_PATH="$HOME/NewProject/PassManager2/data/passdb.sqlite"
export PASSWORD_MANAGER_LOG_PATH="$HOME/NewProject/PassManager2/log.txt"
```


## Build

A C++17-compatible compiler and OpenSSL 3.x are required.

```bash
g++ -std=c++17 -Iinclude -I/opt/homebrew/opt/openssl@3/include src/*.cpp -L/opt/homebrew/opt/openssl@3/lib -lssl -lcrypto -lsqlite3 -o bin/PasswordManager
```


## Usage

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

Then run:

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

Then run:

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
  src/EncryptionManager.cpp \
  -Iinclude \
  -I/opt/homebrew/opt/openssl@3/include \
  -L/opt/homebrew/opt/openssl@3/lib \
  -lssl -lcrypto -lsqlite3 \
  -o bin/test_account_manager
```

Then run:

```bash
./bin/test_account_manager
```

## Roadmap

### completed
- CLI interface
- AES-GCM encryption
- User + Account management
- Environment configuration
- Catch2 tests

### in progress
-  REST API backend
-  Web interface