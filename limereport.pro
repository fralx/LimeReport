TEMPLATE = subdirs
CONFIG += zint
contains(CONFIG, zint){
    SUBDIRS += 3rdparty
}

SUBDIRS += \
        limereport

CONFIG   += ordered

SUBDIRS += demo_r1



