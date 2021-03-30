set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(SOURCES
    main.cpp
    widget.h
    widget.cpp
)

add_executable(widget WIN32 ${SOURCES})

target_link_libraries(widget PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    wangwenx190::QtLottie
)
