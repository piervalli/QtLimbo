

-----

# 🚀 QTURSO: The Qt Driver for Turso (Turso)

A **`QSqlDriver`** plugin to connect Qt/C++ applications to the Turso distributed database (based on Turso).



-----

## 💡 1. Introduction and Project Goals

The **Qt SQL** module provides a database-agnostic interface (`QSqlDatabase`, `QSqlQuery`). However, to connect to systems like **Turso**, a dedicated driver is required.

**QTURSO** bridges this gap, extending Qt SQL capabilities:

  * Enables Qt applications to access a **distributed** and **highly available** database.
  * Leverages the advantages of **Turso** (SQLite rewritten in Rust).


-----

## 🛠️ 2. Architecture and Implementation

The QTURSO driver is a plugin that implements the following Qt SQL components:

### A. QTursoDriver (`QSqlDriver`)

Manages database connection and metadata.

### B. QTursoResult (`QSqlResult`)

Handles SQL statement execution and result set management.

  * **Query Execution:** Sends the SQL statement to the Turso server.
  * **Data Mapping:** Converts the response into Qt's native **`QVariant`** types.

-----

## 📌 3. Quick Start Guide

### 3.1. Adding the Driver

After compiling and installing the `qturso.so`/`.dll` plugin, use it like any Qt driver.
Additionally, you need to distribute the `turso_sqlite3.so/.dll` library:

```cpp
#include <QSqlDatabase>
#include <QDebug>

// Add the database specifying the QTURSO driver name
QSqlDatabase db = QSqlDatabase::addDatabase("QTURSO"); 
```

### 3.2. Configure and Open the Connection
You need to specify your Turso database file path:

```cpp
db.setDatabaseName("data/db.sqlite");
if (!db.open()) {
    qCritical() << "ERROR: Unable to connect to Turso:" << db.lastError().text();
} else {
    qInfo() << "SUCCESS: QTURSO connection established.";
}
```

### 3.3. Executing a Query
Queries are executed in the standard way with QSqlQuery:

```cpp
QSqlQuery query(db);

if (query.exec("SELECT id FROM test")) {
    while (query.next()) {
        int id = query.value("id").toInt();
        qDebug() << "id:" << id << "Nome:" << name;
    }
} else {
    qWarning() << "Errore Query:" << query.lastError().text();
}
```

-----


## 🛣️ 5. Development Status and Roadmap

### ✅ Current Features

  * Qt5 required, tested on Qt 5.15.16
  * Connection and disconnection
  * Execution of simple DML/DDL queries (SELECT, INSERT, CREATE TABLE, etc.)
  * Basic data type mapping (INTEGER, TEXT, REAL, BLOB, NULL)
  * Prepared statements with parameter binding
  * Transaction support (BEGIN, COMMIT, ROLLBACK)

### 🔜 Next Steps (Roadmap)


1. **Unit test** 
2. **Advanced Prepared Statements:** Optimized implementation of bindings for `QSqlQuery::prepare()`
3. **Multi-concurrency Testing:** Multi-threaded write testing with `BEGIN CONCURRENT`
4. **Benchmarks:** Database speed testing and performance comparisons
5. **Remote Connection Support:** Connect to Turso Cloud databases
6. **Embedded Replicas:** Support for local replicas with sync
-----

## 📋 6. API Examples

### Creating a Table

```cpp
QSqlQuery query(db);
if(!query.exec("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, email TEXT)"))
{
    qDebug() << query.lastError().text();
}
```

### Inserting Data

```cpp
QSqlQuery query(db);
query.prepare("INSERT INTO users (name, email) VALUES (?, ?)");
query.addBindValue("John Doe");
query.addBindValue("john@example.com");
if(!query.exec())
{
  qDebug() << query.lastError().text();
}
```

### Updating Data

```cpp
QSqlQuery query(db);
query.prepare("UPDATE users SET email = ? WHERE id = ?");
query.addBindValue("newemail@example.com");
query.addBindValue(1);
if(!query.exec())
{
  qDebug() << query.lastError().text();
}
```

### Transactions

```cpp
db.transaction();

QSqlQuery query(db);
bool ok = true;
ok= query.exec("INSERT INTO users (name, email) VALUES ('Alice', 'alice@example.com')");

if (ok) {
    db.commit();
} else {
    db.rollback();
}
```
-----

## 🐛 7. Known Issues and Limitations

- Remote Turso Cloud connections not yet implemented
- `BEGIN CONCURRENT` support planned for future releases
- Some advanced SQLite features may not be fully tested
- UTF-16 SQLite functions not supported (uses UTF-8 only)

-----

## 📄 8. License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

-----

## 🤝 9. Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

-----

## 📧 10. Contact and Support

- **Issues:** [GitHub Issues](https://github.com/piervalli/QtLimbo/issues)


-----

### ⚠️ Note

**This driver is currently in development and intended for testing purposes only. Do not use in production environments.**


