CONFIG(release, debug|release):DEFINES *= NDEBUG
QT_FOR_CONFIG += core-private
#DEFINES += SQLITE_ENABLE_COLUMN_METADATA SQLITE_OMIT_COMPLETE SQLITE_ENABLE_WAL2  SQLITE_ENABLE_FTS3 SQLITE_ENABLE_FTS3_PARENTHESIS SQLITE_ENABLE_FTS5 SQLITE_ENABLE_RTREE SQLITE_ENABLE_JSON1
# !contains(CONFIG, largefile):DEFINES += SQLITE_DISABLE_LFS
# qtConfig(posix_fallocate): DEFINES += HAVE_POSIX_FALLOCATE=1
# winrt {
#     DEFINES += SQLITE_OS_WINRT
#     QMAKE_CFLAGS += -Gy
# }
# qnx: DEFINES += _QNX_SOURCE
# !win32:!winrt:!winphone: DEFINES += HAVE_USLEEP=1
# qtConfig(dlopen) {
#     QMAKE_USE += libdl
# } else {
    # DEFINES += SQLITE_OMIT_LOAD_EXTENSION
# }
integrity: QMAKE_CFLAGS += -include qplatformdefs.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/turso/win64/ -lturso_sqlite3
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/turso/win64/ -lturso_sqlite3

INCLUDEPATH += $$PWD/turso/include
DEPENDPATH += $$PWD/turso/include
