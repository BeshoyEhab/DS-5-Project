cmake_minimum_required(VERSION 3.16)

project(FastWriterPro)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

set(SOURCES
    main.cpp
    autocompleteapp.cpp
    inputfield.cpp
    hoverablebutton.cpp
)

set(HEADERS
    autocompleteapp.h
    inputfield.h
    hoverablebutton.h
)

add_executable(FastWriterPro ${SOURCES} ${HEADERS})

target_link_libraries(FastWriterPro PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
)

# Set default properties
set_target_properties(FastWriterPro PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Set bundle identifier for Qt versions before 6.1.0
if(Qt6_VERSION VERSION_LESS "6.1.0")
    set_target_properties(FastWriterPro PROPERTIES
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.untitled3
    )
endif()

include(GNUInstallDirs)
install(TARGETS FastWriterPro
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
