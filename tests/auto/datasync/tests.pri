TEMPLATE = app

QT = core testlib datasync-private

CONFIG   += console
CONFIG   -= app_bundle

DEFINES += SRCDIR=\\\"$$_PRO_FILE_PWD_/\\\"

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../TestLib/release/ -lTestLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../TestLib/debug/ -lTestLib
else:unix: LIBS += -L$$OUT_PWD/../TestLib/ -lTestLib

INCLUDEPATH += $$PWD/TestLib
DEPENDPATH += $$PWD/TestLib

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TestLib/release/libTestLib.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TestLib/debug/libTestLib.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TestLib/release/TestLib.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../TestLib/debug/TestLib.lib
else:unix: PRE_TARGETDEPS += $$OUT_PWD/../TestLib/libTestLib.a

INCLUDEPATH += $$PWD/../../../src/datasync/messages

mac: QMAKE_LFLAGS += '-Wl,-rpath,\'$$OUT_PWD/../../../../lib\''

DEFINES += KEYSTORE_PATH=\\\"$$OUT_PWD/../../../../plugins/keystores/\\\"
