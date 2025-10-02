#include <QtSql>
#include <QCoreApplication>
#include <QTemporaryDir>

void setupLogging()
{
    qSetMessagePattern(
        "[%{time yyyy-MM-dd HH:mm:ss.zzz}] "
        "[%{type}] "
        "[%{file}:%{line}] "
        "[%{function}] "
        "[TID:%{threadid}] "
        "%{if-category}[%{category}] %{endif}"
        "%{message}"
        );
}
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    Q_UNUSED(app)
    setupLogging();
    qDebug() << "QTURSO isDriverAvailable" << QSqlDatabase::isDriverAvailable("QTURSO");

    Q_ASSERT(QSqlDatabase::isDriverAvailable("QTURSO"));

    // QTemporaryDir tmp;
    // Q_ASSERT(tmp.isValid());
    QString databasePath = "local.db";
    qDebug() << "DB File Path is:" << databasePath;


    QSqlDatabase dbconn = QSqlDatabase::addDatabase("QTURSO");
    dbconn.setDatabaseName(databasePath);
    ///dbconn.setConnectOptions("");//TODO
    bool ok =dbconn.open();
    qDebug() << ok <<"isOpen(): " << dbconn.isOpen() << "isOpenError" <<dbconn.isOpenError();
    if (!dbconn.isOpen())
    {
        qDebug() << "Connection failed: " << dbconn.lastError().driverText();
        exit(-1);
    }


    QSqlQuery query(dbconn);
    qDebug() << dbconn.transaction();
    qDebug() << dbconn.commit();
    // dbconn.close();
    if(!query.exec("SELECT 1;"))
    {
         qDebug() << query.lastError().text();
    }
    while (query.next()) {
        qDebug() << query.value(0);
    }
    if(!query.exec(" select sqlite_version();"))
    {
        qDebug() << query.lastError().text();
    }
    while (query.next()) {
        qDebug() << "version"<<query.value(0);
    }

    if(!query.exec("CREATE TABLE IF NOT EXISTS test (id int, name varchar)"))
    {
        qDebug() << query.lastError().text();
    }

    int i = 0;
    dbconn.transaction();
    do
    {
        if(!query.exec(QStringLiteral("insert into test values (%1, 'PRODUCT %1')").arg(i++)))
        {
            qCritical() << query.lastError().text();
            break;
        }
    }while(i<10);
    dbconn.commit();
    if(!query.exec(" select id from test LIMIT 1;"))
    {
        qDebug() << query.lastError().text();
    }
    while (query.next()) {
        qDebug() << "version"<<query.record();
    }

    if(!query.exec("pragma table_info('test');"))
    {

        qCritical() << query.lastError().text();
    }
    auto recordEmpty=query.record();
    auto columnsSize = recordEmpty.count();
    qDebug() << "columns" << columnsSize;

    while (query.next())
    {
        QSqlRecord record=query.record();
        for (int i = 0; i < columnsSize; ++i)
        {
            const auto field = record.field(i);
            qDebug() << record.fieldName(i) << field.type() << field.tableName();
        }
    }

    dbconn.close();

    return 0;
}
