cmake_minimum_required(VERSION 3.9)

project(SimpleTextEdit VERSION 0.5.0 LANGUAGES CXX DESCRIPTION "Simple Qt Text Editor")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets PrintSupport LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets PrintSupport LinguistTools)

set(TS_FILES
        SimpleTextEdit_en.ts
        SimpleTextEdit_es.ts
        SimpleTextEdit_fr.ts
        SimpleTextEdit_ko.ts
)

set(SOURCE_FILES
        main.cpp
        findflags.hpp
        mainwindow.cpp
        mainwindow.hpp
        mainwindow.ui
        aboutdialog.cpp
        aboutdialog.hpp
        aboutdialog.ui
        findreplacedialog.cpp
        findreplacedialog.hpp
        findreplacedialog.ui
        maintextedit.hpp
        maintextedit.cpp
)

set(PROJECT_SOURCES
        ${SOURCE_FILES}
        images.qrc
        ${TS_FILES}
)

configure_file(buildinfo.hpp.in headers/buildinfo.hpp)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(SimpleTextEdit
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET SimpleTextEdit APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation

    qt_add_translations(SimpleTextEdit TS_FILES ${TS_FILES} SOURCES ${SOURCE_FILES})
else()
    if(ANDROID)
        add_library(SimpleTextEdit SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(SimpleTextEdit
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_include_directories(SimpleTextEdit PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/headers)
target_link_libraries(SimpleTextEdit PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
                                     PRIVATE Qt${QT_VERSION_MAJOR}::PrintSupport)

set_target_properties(SimpleTextEdit PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS SimpleTextEdit
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(SimpleTextEdit)
endif()
