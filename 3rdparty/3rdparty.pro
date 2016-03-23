ZINT_PATH = $$PWD/zint-2.4.4/
ZINT_VERSION = 2.4.4
INCLUDEPATH += $${ZINT_PATH}/backend $${ZINT_PATH}/backend_qt4
DEPENDPATH  += $${ZINT_PATH}/backend $${ZINT_PATH}/backend_qt4
include($${ZINT_PATH}/backend_qt4/Zint.pro)

