# LimeReport 3rd-party modules

find_package(PNG)

set(ZINT_FILES
    zint-2.6.1/backend/2of5.c
    zint-2.6.1/backend/auspost.c
    zint-2.6.1/backend/aztec.c
    zint-2.6.1/backend/bmp.c
    zint-2.6.1/backend/codablock.c
    zint-2.6.1/backend/code.c
    zint-2.6.1/backend/code1.c
    zint-2.6.1/backend/code128.c
    zint-2.6.1/backend/code16k.c
    zint-2.6.1/backend/code49.c
    zint-2.6.1/backend/common.c
    zint-2.6.1/backend/composite.c
    zint-2.6.1/backend/dllversion.c
    zint-2.6.1/backend/dmatrix.c
    zint-2.6.1/backend/dotcode.c
    zint-2.6.1/backend/eci.c
    zint-2.6.1/backend/emf.c
    zint-2.6.1/backend/gif.c
    zint-2.6.1/backend/gridmtx.c
    zint-2.6.1/backend/gs1.c
    zint-2.6.1/backend/hanxin.c
    zint-2.6.1/backend/imail.c
    zint-2.6.1/backend/large.c
    zint-2.6.1/backend/library.c
    zint-2.6.1/backend/libzint.rc
    zint-2.6.1/backend/maxicode.c
    zint-2.6.1/backend/medical.c
    zint-2.6.1/backend/pcx.c
    zint-2.6.1/backend/pdf417.c
    zint-2.6.1/backend/plessey.c
    zint-2.6.1/backend/png.c
    zint-2.6.1/backend/postal.c
    zint-2.6.1/backend/ps.c
    zint-2.6.1/backend/qr.c
    zint-2.6.1/backend/raster.c
    zint-2.6.1/backend/reedsol.c
    zint-2.6.1/backend/render.c
    zint-2.6.1/backend/rss.c
    zint-2.6.1/backend/svg.c
    zint-2.6.1/backend/telepen.c
    zint-2.6.1/backend/tif.c
    zint-2.6.1/backend/upcean.c
    zint-2.6.1/backend_qt/qzint.cpp
  )

if (ENABLE_ZINT)

  add_library(QZint STATIC  ${ZINT_FILES})

  target_include_directories(QZint PUBLIC zint-2.6.1/backend)
  target_include_directories(QZint PUBLIC zint-2.6.1/backend_qt)

  target_link_libraries(QZint PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
  )

  target_compile_definitions(QZint PUBLIC -DQZINT_STATIC_BUILD)

  if(PNG_FOUND)
    target_link_libraries(QZint PRIVATE PNG::PNG)
  else(PNG_FOUND)
    target_compile_definitions(QZint -DNO_PNG)
  endif(PNG_FOUND)

endif(ENABLE_ZINT)
