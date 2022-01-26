# LimeReport 3rd-party modules

find_package(PNG)

set(CMAKE_AUTOMOC ON)

set(ZINT_FILES
    zint-2.10.0/backend/2of5.c
    zint-2.10.0/backend/auspost.c
    zint-2.10.0/backend/aztec.c
    zint-2.10.0/backend/bmp.c
    zint-2.10.0/backend/codablock.c
    zint-2.10.0/backend/code.c
    zint-2.10.0/backend/code1.c
    zint-2.10.0/backend/code128.c
    zint-2.10.0/backend/code16k.c
    zint-2.10.0/backend/code49.c
    zint-2.10.0/backend/common.c
    zint-2.10.0/backend/composite.c
    zint-2.10.0/backend/dmatrix.c
    zint-2.10.0/backend/dotcode.c
    zint-2.10.0/backend/eci.c
    zint-2.10.0/backend/emf.c
    zint-2.10.0/backend/gb18030.c
    zint-2.10.0/backend/gb2312.c
    zint-2.10.0/backend/general_field.c
    zint-2.10.0/backend/gif.c
    zint-2.10.0/backend/gridmtx.c
    zint-2.10.0/backend/gs1.c
    zint-2.10.0/backend/hanxin.c
    zint-2.10.0/backend/imail.c
    zint-2.10.0/backend/large.c
    zint-2.10.0/backend/library.c
    zint-2.10.0/backend/mailmark.c
    zint-2.10.0/backend/maxicode.c
    zint-2.10.0/backend/medical.c
    zint-2.10.0/backend/output.c
    zint-2.10.0/backend/pcx.c
    zint-2.10.0/backend/pdf417.c
    zint-2.10.0/backend/plessey.c
    zint-2.10.0/backend/png.c
    zint-2.10.0/backend/postal.c
    zint-2.10.0/backend/ps.c
    zint-2.10.0/backend/qr.c
    zint-2.10.0/backend/raster.c
    zint-2.10.0/backend/reedsol.c
    zint-2.10.0/backend/rss.c
    zint-2.10.0/backend/sjis.c
    zint-2.10.0/backend/svg.c
    zint-2.10.0/backend/telepen.c
    zint-2.10.0/backend/tif.c
    zint-2.10.0/backend/ultra.c
    zint-2.10.0/backend/upcean.c
    zint-2.10.0/backend/vector.c
    zint-2.10.0/backend/dllversion.c
    zint-2.10.0/backend_qt/qzint.cpp
    zint-2.10.0/backend_qt/qzint.h
  )

if (ENABLE_ZINT)

  add_library(QZint STATIC ${ZINT_FILES})

  target_include_directories(QZint PUBLIC zint-2.10.0/backend)
  target_include_directories(QZint PUBLIC zint-2.10.0/backend_qt)

  target_link_libraries(QZint PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
  )

  target_compile_definitions(QZint PUBLIC -DQZINT_STATIC_BUILD)

  if(PNG_FOUND)
    target_link_libraries(QZint PRIVATE PNG::PNG)
  else(PNG_FOUND)
    target_compile_definitions(QZint PRIVATE -DNO_PNG)
  endif(PNG_FOUND)

endif(ENABLE_ZINT)
