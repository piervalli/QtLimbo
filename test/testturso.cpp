#include "testturso.h"
#include <QDebug>
TestTurso::TestTurso(QObject *parent)
    : QObject{parent},testDbPath("test_turso.db")
{}

TestTurso::~TestTurso()
{

}

void TestTurso::initTestCase()
{
    qInfo() << "=== Inizio Test Suite QTURSO ===";

    // Rimuovi database di test se esiste
    if (QFile::exists(testDbPath)) {
        QFile::remove(testDbPath);
    }

    // Verifica che il driver QTURSO sia disponibile
    if (!QSqlDatabase::isDriverAvailable("QTURSO")) {
        QFAIL("Driver QTURSO non disponibile!");
    }

    if(!QSqlDatabase::contains("TURSO"))
        QSqlDatabase::addDatabase("QTURSO","TURSO");
    auto db = QSqlDatabase::database("TURSO");
    db.setDatabaseName(testDbPath);

    QVERIFY2(db.open(), qPrintable(db.lastError().text()));
    QVERIFY(db.isOpen());
    qInfo() << "Driver QTURSO disponibile";
}

void TestTurso::cleanupTestCase()
{
    auto db = QSqlDatabase::database("TURSO");
    if (db.isOpen()) {
        db.close();
    }
    QSqlDatabase::removeDatabase(m_defaultConnection);

    qInfo() << "=== Fine Test Suite QTURSO ===";
}

void TestTurso::init()
{
    // Eseguito prima di ogni test
}

void TestTurso::cleanup()
{
    // Eseguito dopo ogni test
}

// ============================================================================
// TEST CONNESSIONE
// ============================================================================

void TestTurso::testConnection()
{
    auto db = QSqlDatabase::database("TURSO");
    db.setDatabaseName(testDbPath);

    QVERIFY2(db.open(), qPrintable(db.lastError().text()));
    QVERIFY(db.isOpen());

    qInfo() << "✓ Connessione stabilita con successo";
}

void TestTurso::testConnectionFailure()
{
    QSqlDatabase tempDb = QSqlDatabase::addDatabase("QTURSO", "temp_connection");
    tempDb.setDatabaseName("/invalid/path/to/database.db");

    // Dovrebbe fallire
    QVERIFY(!tempDb.open());

    QSqlDatabase::removeDatabase("temp_connection");
    qInfo() << "✓ Test fallimento connessione OK";
}

// ============================================================================
// TEST CREAZIONE TABELLE
// ============================================================================

void TestTurso::testCreateTable()
{
    auto db = QSqlDatabase::database("TURSO");
    QVERIFY(db.isOpen());

    QSqlQuery query(db);
    QVERIFY2(query.exec("DROP TABLE IF EXISTS test_simple"), qPrintable(query.lastError().text()));

    QString sql = "CREATE TABLE IF NOT EXISTS test_simple ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "name TEXT NOT NULL"
                  ")";

    QVERIFY2(query.exec(sql), qPrintable(query.lastError().text()));

    qInfo() << "✓ Tabella semplice creata";
}

void TestTurso::testCreateTableWithAllTypes()
{
    auto db = QSqlDatabase::database("TURSO");
    QVERIFY(db.isOpen());

    QSqlQuery query(db);
    QVERIFY2(query.exec("DROP TABLE IF EXISTS test_all_types"), qPrintable(query.lastError().text()));
    QString sql = "CREATE TABLE IF NOT EXISTS test_all_types ("
                  "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                  "col_integer INTEGER, "
                  "col_text TEXT, "
                  "col_real REAL, "
                  "col_blob BLOB, "
                  "col_null INTEGER, "
                  "col_bool BOOLEAN, "
                  "col_datetime TEXT"
                  ")";

    QVERIFY2(query.exec(sql), qPrintable(query.lastError().text()));

    qInfo() << "✓ Tabella con tutti i tipi creata";
}

// ============================================================================
// TEST TIPI DI DATI
// ============================================================================

void TestTurso::testIntegerType()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    // Insert
    QVERIFY(query.exec("INSERT INTO test_all_types (col_integer) VALUES (42)"));

    // Select
    QVERIFY(query.exec("SELECT col_integer FROM test_all_types WHERE col_integer = 42"));
    QVERIFY(query.next());

    QVariant value = query.value(0);
    QCOMPARE(value.type(), QVariant::LongLong);
    QCOMPARE(value.toLongLong(), 42LL);

    qInfo() << "✓ Tipo INTEGER: valore =" << value;
}

void TestTurso::testTextType()
{
    auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    QString testText = "Hello QTURSO";
    query.prepare("INSERT INTO test_all_types (col_text) VALUES (:col_text)");
    query.bindValue(0,testText);
    QVERIFY(query.exec());

    QVERIFY(query.exec("SELECT col_text FROM test_all_types WHERE col_text = 'Hello QTURSO'"));
    QVERIFY(query.next());

    QVariant value = query.value(0);
    QCOMPARE(value.type(), QVariant::String);
    QCOMPARE(value.toString(), testText);

    qInfo() << "✓ Tipo TEXT: valore =" << value;
}

void TestTurso::testRealType()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    double testReal = 3.14159;
    query.prepare("INSERT INTO test_all_types (col_real) VALUES (?)");
    query.addBindValue(testReal);
    QVERIFY(query.exec());

    QVERIFY(query.exec("SELECT col_real FROM test_all_types WHERE col_real > 3.0"));
    QVERIFY(query.next());

    QVariant value = query.value(0);
    QCOMPARE(value.type(), QVariant::Double);
    QCOMPARE(value.toDouble(), testReal);

    qInfo() << "✓ Tipo REAL: valore =" << value;
}

void TestTurso::testBlobType()
{
    auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    QByteArray testBlob;
    testBlob.append(static_cast<char>(0x61)); // 'a'
    testBlob.append(static_cast<char>(0x00)); // null byte
    testBlob.append(static_cast<char>(0x62)); // 'b'

    query.prepare("INSERT INTO test_all_types (col_blob) VALUES (?)");
    query.addBindValue(testBlob);
    QVERIFY2(query.exec(), qPrintable(query.lastError().text()));

    QVERIFY(query.exec("SELECT col_blob FROM test_all_types WHERE col_blob IS NOT NULL"));
    QVERIFY(query.next());

    QVariant value = query.value(0);
    QCOMPARE(value.type(), QVariant::ByteArray);
    QCOMPARE(value.toByteArray(), testBlob);

    qInfo() << "✓ Tipo BLOB: size =" << value.toByteArray().size()
            << "hex =" << value.toByteArray().toHex();
}

void TestTurso::testNullType()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    query.prepare("INSERT INTO test_all_types (col_null) VALUES (?)");
    query.addBindValue(QVariant(QVariant::Int));
    QVERIFY(query.exec());

    QVERIFY(query.exec("SELECT col_null FROM test_all_types WHERE col_null IS NULL"));
    QVERIFY(query.next());

    QVariant value = query.value(0);
    QVERIFY(value.isNull());

    qInfo() << "✓ Tipo NULL: isNull =" << value.isNull();
}

void TestTurso::testBooleanType()
{
    auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    query.prepare("INSERT INTO test_all_types (col_bool) VALUES (?)");
    query.addBindValue(true);
    QVERIFY(query.exec());

    QVERIFY(query.exec("SELECT col_bool FROM test_all_types WHERE col_bool = 1"));
    QVERIFY(query.next());

    QVariant value = query.value(0);
    QCOMPARE(value.toBool(), true);

    qInfo() << "✓ Tipo BOOLEAN: valore =" << value.toBool();
}

void TestTurso::testDateTimeType()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    QDateTime now = QDateTime::currentDateTime();
    QString dateTimeStr = now.toString(Qt::ISODate);

    query.prepare("INSERT INTO test_all_types (col_datetime) VALUES (?)");
    query.addBindValue(dateTimeStr);
    QVERIFY(query.exec());

    QVERIFY(query.exec("SELECT col_datetime FROM test_all_types WHERE col_datetime IS NOT NULL ORDER BY id DESC LIMIT 1"));
    QVERIFY(query.next());

    QString retrieved = query.value(0).toString();
    QCOMPARE(retrieved, dateTimeStr);

    qInfo() << "✓ Tipo DATETIME: valore =" << retrieved;
}

// ============================================================================
// TEST NOMI CAMPI E METADATA
// ============================================================================

void TestTurso::testFieldNames()
{
    auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);
    QVERIFY(query.exec("SELECT id, col_integer, col_text FROM test_all_types LIMIT 1"));

    QSqlRecord record = query.record();

    QCOMPARE(record.count(), 3);
    QCOMPARE(record.fieldName(0), QString("id"));
    QCOMPARE(record.fieldName(1), QString("col_integer"));
    QCOMPARE(record.fieldName(2), QString("col_text"));

    qInfo() << "✓ Nomi campi verificati:";
    for (int i = 0; i < record.count(); ++i) {
        qInfo() << "  -" << record.fieldName(i);
    }
}

void TestTurso::testFieldTypes()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    // Inserisci dati di test
    QVERIFY(query.exec("INSERT INTO test_all_types (col_integer, col_text, col_real) "
                       "VALUES (123, 'test', 45.67)"));

    QVERIFY(query.exec("SELECT col_integer, col_text, col_real FROM test_all_types "
                       "WHERE col_integer = 123"));
    QVERIFY(query.next());

    QSqlRecord record = query.record();

    qInfo() << "✓ Tipi campi:";
    for (int i = 0; i < record.count(); ++i) {
        QSqlField field = record.field(i);
        qInfo() << "  -" << field.name()
                << ": SQLite type =" << field.type()
                << ", value type =" << query.value(i).typeName();
    }

    // Verifica tipi
    QCOMPARE(query.value(0).type(), QVariant::LongLong);
    QCOMPARE(query.value(1).type(), QVariant::String);
    QCOMPARE(query.value(2).type(), QVariant::Double);
}

void TestTurso::testFieldCount()
{
    auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);
    QVERIFY(query.exec("SELECT * FROM test_all_types LIMIT 1"));

    QSqlRecord record = query.record();
    int fieldCount = record.count();

    QVERIFY(fieldCount > 0);
    qInfo() << "✓ Numero campi in test_all_types:" << fieldCount;
}

// ============================================================================
// TEST VALORI E CONVERSIONI
// ============================================================================

void TestTurso::testValueRetrieval()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    // Inserisci valori di test
    QVERIFY(query.exec("INSERT INTO test_all_types (col_integer, col_text, col_real) "
                       "VALUES (999, 'conversion_test', 88.88)"));

    QVERIFY(query.exec("SELECT col_integer, col_text, col_real FROM test_all_types "
                       "WHERE col_integer = 999"));
    QVERIFY(query.next());

    // Test recupero per indice
    QCOMPARE(query.value(0).toInt(), 999);
    QCOMPARE(query.value(1).toString(), QString("conversion_test"));
    QCOMPARE(query.value(2).toDouble(), 88.88);

    // Test recupero per nome campo
    QSqlRecord rec = query.record();
    int idxInteger = rec.indexOf("col_integer");
    int idxText = rec.indexOf("col_text");
    int idxReal = rec.indexOf("col_real");

    QCOMPARE(query.value(idxInteger).toInt(), 999);
    QCOMPARE(query.value(idxText).toString(), QString("conversion_test"));
    QCOMPARE(query.value(idxReal).toDouble(), 88.88);

    qInfo() << "✓ Recupero valori per indice e nome OK";
}

void TestTurso::testValueConversion()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    QVERIFY(query.exec("INSERT INTO test_all_types (col_integer) VALUES (777)"));
    QVERIFY(query.exec("SELECT col_integer FROM test_all_types WHERE col_integer = 777"));
    QVERIFY(query.next());

    QVariant value = query.value(0);

    // Test conversioni
    QCOMPARE(value.toInt(), 777);
    QCOMPARE(value.toLongLong(), 777LL);
    QCOMPARE(value.toString(), QString("777"));
    QCOMPARE(value.toDouble(), 777.0);
    QCOMPARE(value.toBool(), true);  // Non-zero = true

    qInfo() << "✓ Conversioni tipo OK";
}

void TestTurso::testNullValues()
{
    auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    QVERIFY(query.exec("INSERT INTO test_all_types (col_integer, col_text) VALUES (NULL, NULL)"));
    QVERIFY(query.exec("SELECT col_integer, col_text FROM test_all_types WHERE col_integer IS NULL"));
    QVERIFY(query.next());

    QVERIFY(query.value(0).isNull());
    QVERIFY(query.value(1).isNull());

    qInfo() << "✓ Gestione valori NULL OK";
}

// ============================================================================
// TEST PREPARED STATEMENTS
// ============================================================================

void TestTurso::testPreparedStatement()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    query.prepare("INSERT INTO test_all_types (col_integer, col_text, col_real) "
                  "VALUES (?, ?, ?)");
    query.addBindValue(111);
    query.addBindValue("prepared_test");
    query.addBindValue(22.22);

    QVERIFY2(query.exec(), qPrintable(query.lastError().text()));

    // Verifica
    QVERIFY(query.exec("SELECT col_integer, col_text, col_real FROM test_all_types "
                       "WHERE col_integer = 111"));
    QVERIFY(query.next());

    QCOMPARE(query.value(0).toInt(), 111);
    QCOMPARE(query.value(1).toString(), QString("prepared_test"));
    QCOMPARE(query.value(2).toDouble(), 22.22);

    qInfo() << "✓ Prepared statement OK";
}

void TestTurso::testPreparedStatementWithNull()
{
    auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    query.prepare("INSERT INTO test_all_types (col_integer, col_text) VALUES (?, ?)");
    query.addBindValue(555);
    query.addBindValue(QVariant(QVariant::String));  // NULL

    QVERIFY(query.exec());

    QVERIFY(query.exec("SELECT col_text FROM test_all_types WHERE col_integer = 555"));
    QVERIFY(query.next());
    QVERIFY(query.value(0).isNull());

    qInfo() << "✓ Prepared statement con NULL OK";
}

void TestTurso::testMultipleBindings()
{
    auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    query.prepare("INSERT INTO test_all_types (col_integer, col_text, col_real, col_bool) "
                  "VALUES (?, ?, ?, ?)");

    // Primo insert
    query.addBindValue(100);
    query.addBindValue("first");
    query.addBindValue(1.1);
    query.addBindValue(true);
    QVERIFY(query.exec());

    // Secondo insert (riutilizzo prepared statement)
    query.addBindValue(200);
    query.addBindValue("second");
    query.addBindValue(2.2);
    query.addBindValue(false);
    QVERIFY(query.exec());

    // Verifica
    QVERIFY(query.exec("SELECT COUNT(*) FROM test_all_types WHERE col_integer IN (100, 200)"));
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toInt(), 2);

    qInfo() << "✓ Multiple bindings OK";
}

// ============================================================================
// TEST TRANSAZIONI
// ============================================================================

void TestTurso::testTransaction()
{
     auto db = QSqlDatabase::database("TURSO");
    QVERIFY(db.transaction());

    QSqlQuery query(db);
    QVERIFY(query.exec("INSERT INTO test_all_types (col_integer) VALUES (888)"));

    QVERIFY(db.commit());

    // Verifica
    QVERIFY(query.exec("SELECT col_integer FROM test_all_types WHERE col_integer = 888"));
    QVERIFY(query.next());

    qInfo() << "✓ Transaction commit OK";
}

void TestTurso::testTransactionRollback()
{
    auto db = QSqlDatabase::database("TURSO");
    QVERIFY(db.transaction());

    QSqlQuery query(db);
    QVERIFY(query.exec("INSERT INTO test_all_types (col_integer) VALUES (666)"));

    QVERIFY(db.rollback());

    // Verifica che il dato NON sia stato salvato
    QVERIFY(query.exec("SELECT COUNT(*) FROM test_all_types WHERE col_integer = 666"));
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toInt(), 0);

    qInfo() << "✓ Transaction rollback OK";
}

void TestTurso::testTransactionCommit()
{
    auto db = QSqlDatabase::database("TURSO");
    QVERIFY(db.transaction());

    QSqlQuery query(db);
    for (int i = 1; i <= 5; ++i) {
        query.prepare("INSERT INTO test_all_types (col_integer, col_text) VALUES (?, ?)");
        query.addBindValue(1000 + i);
        query.addBindValue(QString("tx_test_%1").arg(i));
        QVERIFY(query.exec());
    }

    QVERIFY(db.commit());

    // Verifica
    QVERIFY(query.exec("SELECT COUNT(*) FROM test_all_types WHERE col_integer BETWEEN 1001 AND 1005"));
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toInt(), 5);

    qInfo() << "✓ Transaction con multiple insert OK";
}

// ============================================================================
// TEST CASI LIMITE
// ============================================================================

void TestTurso::testEmptyString()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    query.prepare("INSERT INTO test_all_types (col_text) VALUES (?)");
    query.addBindValue(QString(""));
    QVERIFY(query.exec());

    QVERIFY(query.exec("SELECT col_text FROM test_all_types WHERE col_text = ''"));
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toString(), QString(""));

    qInfo() << "✓ Stringa vuota OK";
}

void TestTurso::testUnicodeStrings()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    QString unicodeText = "Hello 世界 🚀 Привет";
    query.prepare("INSERT INTO test_all_types (col_text) VALUES (?)");
    query.addBindValue(unicodeText);
    QVERIFY(query.exec());

    QVERIFY(query.exec("SELECT col_text FROM test_all_types WHERE col_text LIKE '%世界%'"));
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toString(), unicodeText);

    qInfo() << "✓ Unicode strings OK:" << unicodeText;
}

void TestTurso::testLargeBlob()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    // Crea un blob di 1MB
    QByteArray largeBlob(1024 * 1024, 'X');

    query.prepare("INSERT INTO test_all_types (col_blob) VALUES (?)");
    query.addBindValue(largeBlob);
    QVERIFY(query.exec());

    QVERIFY(query.exec("SELECT col_blob FROM test_all_types WHERE LENGTH(col_blob) > 1000000"));
    QVERIFY(query.next());

    QByteArray retrieved = query.value(0).toByteArray();
    QCOMPARE(retrieved.size(), largeBlob.size());

    qInfo() << "✓ Large BLOB OK: size =" << retrieved.size();
}

void TestTurso::testSpecialCharacters()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    QString specialText = "Test's \"quoted\" text\nwith\nnewlines\tand\ttabs";
    query.prepare("INSERT INTO test_all_types (col_text) VALUES (?)");
    query.addBindValue(specialText);
    QVERIFY(query.exec());

    QVERIFY(query.exec("SELECT col_text FROM test_all_types WHERE col_text LIKE '%quoted%'"));
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toString(), specialText);

    qInfo() << "✓ Caratteri speciali OK";
}

// ============================================================================
// TEST PERFORMANCE
// ============================================================================

void TestTurso::testBulkInsert()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    QVERIFY(db.transaction());

    QElapsedTimer timer;
    timer.start();

    query.prepare("INSERT INTO test_all_types (col_integer, col_text) VALUES (?, ?)");

    for (int i = 0; i < 1000; ++i) {
        query.addBindValue(2000 + i);
        query.addBindValue(QString("bulk_%1").arg(i));
        QVERIFY(query.exec());
    }

    QVERIFY(db.commit());

    qint64 elapsed = timer.elapsed();

    // Verifica
    QVERIFY(query.exec("SELECT COUNT(*) FROM test_all_types WHERE col_integer BETWEEN 2000 AND 2999"));
    QVERIFY(query.next());
    QCOMPARE(query.value(0).toInt(), 1000);

    qInfo() << "✓ Bulk insert di 1000 record in" << elapsed << "ms";
}

void TestTurso::testMultipleQueries()
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);

    QElapsedTimer timer;
    timer.start();

    for (int i = 0; i < 100; ++i) {
        QVERIFY(query.exec("SELECT COUNT(*) FROM test_all_types"));
        QVERIFY(query.next());
    }

    qint64 elapsed = timer.elapsed();

    qInfo() << "✓ 100 query SELECT in" << elapsed << "ms";
}

// ============================================================================
// HELPER METHODS
// ============================================================================

bool TestTurso::executeQuery(const QString &sql)
{
     auto db = QSqlDatabase::database("TURSO");
    QSqlQuery query(db);
    if (!query.exec(sql)) {
        qWarning() << "Query failed:" << sql;
        qWarning() << "Error:" << query.lastError().text();
        return false;
    }
    return true;
}

void TestTurso::printQueryResults(QSqlQuery &query)
{
    QSqlRecord record = query.record();

    qDebug() << "=== Query Results ===";
    qDebug() << "Columns:" << record.count();

    for (int i = 0; i < record.count(); ++i) {
        qDebug() << "  " << record.fieldName(i);
    }

    int rowCount = 0;
    while (query.next()) {
        rowCount++;
        qDebug() << "Row" << rowCount << ":";
        for (int i = 0; i < record.count(); ++i) {
            qDebug() << "  " << record.fieldName(i) << "=" << query.value(i);
        }
    }

    qDebug() << "Total rows:" << rowCount;
}
