CONFIG += zint
include(./limereport/limereport.pri)
contains(CONFIG, zint){
    include(./qzint.pri)
}
