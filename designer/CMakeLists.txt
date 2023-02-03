project(LRDesigner)

set(CMAKE_AUTOMOC ON)

set(LRDESIGNER_FILES
    designersettingmanager.h
    designersettingmanager.cpp
    main.cpp
    mainicon.rc
)

add_executable(${PROJECT_NAME} ${LRDESIGNER_FILES})

target_link_libraries(${PROJECT_NAME} PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Qml
    limereport-qt${QT_VERSION_MAJOR}
)
