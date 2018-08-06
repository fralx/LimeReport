TEMPLATE = subdirs

!contains(CONFIG, no_zint){
    CONFIG += zint
}

include(common.pri)
contains(CONFIG, zint){
    SUBDIRS += 3rdparty
}

export($$CONFIG)

CONFIG  += ordered
SUBDIRS += \
        initvars.pro \
        limereport \
        demo_r1 \
        demo_r2 \
        designer

!contains(CONFIG, embedded_designer){
!contains(CONFIG, static_build){
SUBDIRS += designer_plugin
}
}

# QMake top level srcdir and builddir
# https://wiki.qt.io/QMake-top-level-srcdir-and-builddir
#Qt4 .qmake.cache.in
#Qt5 .qmake.conf
OTHER_FILES += \
    .qmake.conf \
    .qmake.cache.in
