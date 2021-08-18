TEMPLATE = subdirs

include(common.pri)

CONFIG += ordered

CONFIG(zint) {
    SUBDIRS += 3rdparty
}

SUBDIRS += \
        limereport \
        demo_r1 \
        demo_r2 \
        designer

greaterThan(QT_MAJOR_VERSION, 4) : greaterThan(QT_MINOR_VERSION, 1) {
    SUBDIRS += console
}

!CONFIG(embedded_designer) : !CONFIG(static_build) {
    SUBDIRS += designer_plugin
}
