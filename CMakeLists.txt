cmake_minimum_required(VERSION 3.16)

project(tipstranslate VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools Network)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools Network)


set(CMAKE_PREFIX_PATH "C:/vcpkg/installed/x64-windows")
set(Tesseract_DIR "C:/vcpkg/installed/x64-windows/share/tesseract")
find_package(Tesseract CONFIG REQUIRED)

set(TS_FILES tipstranslate_en_US.ts)

set(PROJECT_SOURCES
        main.cpp
        listenertip.cpp
        listenertip.h
        listenertip.ui
        OverlaySelect.h
        OverlaySelect.cpp
        FloatingPane.h
        FloatingPane.cpp
        OcrWorker.h
        OcrWorker.cpp
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(tipstranslate
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        Translator.h
        Translator.cpp
        README.md
        HotkListener.h
        HotkListener.cpp
        resources.qrc
        config.example.ini

    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(tipstranslate SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(tipstranslate
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(tipstranslate
    PRIVATE
        Qt${QT_VERSION_MAJOR}::Widgets
        Tesseract::libtesseract
        Qt${QT_VERSION_MAJOR}::Network
)

# resto igual
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.tipstranslate)
endif()
set_target_properties(tipstranslate PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS tipstranslate
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(tipstranslate)
endif()
