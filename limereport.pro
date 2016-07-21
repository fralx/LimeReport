TEMPLATE = subdirs
include(common.pri)
contains(CONFIG, zint){
    SUBDIRS += 3rdparty
}

SUBDIRS += \
        limereport

CONFIG   += ordered

SUBDIRS += demo_r1 demo_r2 designer



