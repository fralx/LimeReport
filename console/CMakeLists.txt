project(console)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_SOURCES
        main.cpp
)

add_executable(${PROJECT_NAME} main.cpp ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
        Qt${QT_VERSION_MAJOR}::Core
        limereport-qt${QT_VERSION_MAJOR}
)

