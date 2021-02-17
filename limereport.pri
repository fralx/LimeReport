CONFIG += zint
include(./limereport/limereport.pri)
contains(CONFIG, zint){
    DEFINES+=QZINT_STATIC_BUILD
    include(./qzint.pri)
}
