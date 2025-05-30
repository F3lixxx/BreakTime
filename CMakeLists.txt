cmake_minimum_required(VERSION 3.16)

project(BreakTime VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(app_icon_resource_windows "${CMAKE_CURRENT_SOURCE_DIR}/qss/icon.rc")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Multimedia)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
  if(WIN32)
    qt_add_executable(BreakTime
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        declensionSpinBox.h
        breakwindows.h breakwindows.cpp breakwindows.ui
        style.qrc
        ${app_icon_resource_windows}
    )
else()
  qt_add_executable(BreakTime
      MANUAL_FINALIZATION
      ${PROJECT_SOURCES}
      declensionSpinBox.h
      breakwindows.h breakwindows.cpp breakwindows.ui
      style.qrc
    )
  endif()
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET Breaktime APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(BreakTime SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(BreakTime
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(BreakTime PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.BreakTime)
endif()
set_target_properties(BreakTime PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS BreakTime
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


install(FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/qss/BreakTime.ico
    DESTINATION share/applications
)

# Настройки CPack для DEB-пакета
set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "yenox <y3nohx@gmail.com>")
set(CPACK_PACKAGE_NAME "breaktime")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_DESCRIPTION "A simple break reminder application")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Timer for work breaks")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt${QT_VERSION_MAJOR}widgets${QT_VERSION_MAJOR}, libc6 (>= 2.31)")
set(CPACK_DEBIAN_PACKAGE_SECTION "utils")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst")

include(CPack)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(BreakTime)
endif()
