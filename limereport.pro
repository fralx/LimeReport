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


if(equals(QT_MAJOR_VERSION, 5) : greaterThan(QT_MINOR_VERSION, 1)) | equals(QT_MAJOR_VERSION, 6) {
    SUBDIRS += console \
        lrdview

}

#if(equals(QT_MAJOR_VERSION, 5) : greaterThan(QT_MINOR_VERSION, 1)) {
#    SUBDIRS += lrdview
#}

!CONFIG(embedded_designer) : !CONFIG(static_build) {
    SUBDIRS += designer_plugin
}
