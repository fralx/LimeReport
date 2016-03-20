#QMAKE_CFLAGS += -std=c99
ZINT_PATH = $$PWD/zint-2.4.4/
ZINT_VERSION = 2.4.4
#include(qzint.pri)
#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../zint-2.4.3/build-backend_dll-Desktop_Qt_5_5_0_MSVC2010_32bit-Release/release/ -lQtZint22
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../zint-2.4.3/build-backend_dll-Desktop_Qt_5_5_0_MSVC2010_32bit-Release/debug/ -lQtZint22
INCLUDEPATH += $${ZINT_PATH}/backend $${ZINT_PATH}/backend_qt4
DEPENDPATH  += $${ZINT_PATH}/backend $${ZINT_PATH}/backend_qt4
include($${ZINT_PATH}/backend_qt4/Zint.pro)

