set(CMAKE_AUTOMOC ON)

set(LRDESIGNER_FILES
    designersettingmanager.h
    designersettingmanager.cpp
    main.cpp
    mainicon.rc
)

add_executable(LRDesigner ${LRDESIGNER_FILES})

target_include_directories(LRDesigner PUBLIC ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(LRDesigner PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Qml
    ${PROJECT_NAME}
)
