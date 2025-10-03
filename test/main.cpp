#include "testturso.h"
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
    TestTurso tTurso;
    auto status = QTest::qExec(&tTurso, argc, argv);
    return status;
}
