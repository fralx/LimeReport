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
        limereport \
        demo_r1 \
        demo_r2 \
        console \
        designer

!contains(CONFIG, embedded_designer){
!contains(CONFIG, static_build){
SUBDIRS += designer_plugin
}
}
