ZINT_PATH = $$PWD/zint-2.6.1/
ZINT_VERSION = 2.6.1
INCLUDEPATH += $${ZINT_PATH}/backend $${ZINT_PATH}/backend_qt
DEPENDPATH  += $${ZINT_PATH}/backend $${ZINT_PATH}/backend_qt
include($${ZINT_PATH}/backend_qt/backend_qt.pro)
