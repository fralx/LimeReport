ZINT_PATH = $$PWD/zint-2.10.0
ZINT_VERSION = 2.10.0
INCLUDEPATH += $${ZINT_PATH}/backend $${ZINT_PATH}/backend_qt
DEPENDPATH  += $${ZINT_PATH}/backend $${ZINT_PATH}/backend_qt
include($${ZINT_PATH}/backend_qt/backend_qt.pro)

CONFIG -= warn_on
CONFIG += warn_off
