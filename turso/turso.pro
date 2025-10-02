QT += core core-private sql-private
TARGET = qturso
QTPLUGIN += qturso
TEMPLATE = lib


QT_FOR_CONFIG += sqldrivers-private

CONFIG  += c++14 plugin

HEADERS += $$PWD/qsql_turso_p.h
SOURCES += $$PWD/qsql_turso.cpp \
    $$PWD/smain.cpp

# qtConfig(system-sqlite) {
#     QMAKE_USE += sqlite
# } else {
#     include($$PWD/../../../3rdparty/sqlite.pri)
# }
include($$PWD/3rdparty/turso.pri)

OTHER_FILES += turso.json

# LIBS += $$QT_LFLAGS_SQLITE
# QMAKE_CXXFLAGS *= $$QT_CFLAGS_SQLITE

QMAKE_CXXFLAGS += -DSQLITE_ENABLE_WAL2


PLUGIN_CLASS_NAME = QTursoDriverPlugin

PLUGIN_TYPE = sqldrivers
#include(../qsqldriverbase.pri)

win32:QMAKE_CFLAGS += -march=native

DISTFILES += \
    ../readme.md
