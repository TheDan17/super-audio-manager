cmake_minimum_required(VERSION 3.16)

project(audio-lib-super VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Svg Multimedia)

set(PROJECT_SOURCES
        main.cpp
        ui/mainwindow.cpp
        ui/mainwindow.h
        ui/mainwindow.ui

        ui/module_panels/catalogpanelwidget.h
        ui/module_panels/catalogpanelwidget.cpp
        ui/module_panels/catalogpanelwidget.ui

        ui/qt_items/modulecataloglistitem.h
        ui/qt_items/modulecataloglistitem.cpp
        ui/qt_items/modulecataloglistitem.ui

        res/res.qrc
        items/modulemetainfo.h

        mockup/mockdataprovider.h
        mockup/mockdataprovider.cpp

        ui/module_panels/playerpanelwidget.h
        ui/module_panels/playerpanelwidget.cpp
        ui/module_panels/playerpanelwidget.ui
)

qt_add_executable(audio-lib-super
    MANUAL_FINALIZATION
    ${PROJECT_SOURCES}
    ui/modal_widgets/filtereditdialog.h ui/modal_widgets/filtereditdialog.cpp ui/modal_widgets/filtereditdialog.ui
    ui/modal_widgets/filtereditcomplexmockup.h ui/modal_widgets/filtereditcomplexmockup.cpp ui/modal_widgets/filtereditcomplexmockup.ui
    ui/modal_widgets/appsettingsdialog.h ui/modal_widgets/appsettingsdialog.cpp ui/modal_widgets/appsettingsdialog.ui
    ui/modal_widgets/modulemanagerwidget.h ui/modal_widgets/modulemanagerwidget.cpp ui/modal_widgets/modulemanagerwidget.ui
    core/api.h
    core/core.h
    modules/impl_modules.h
    ui/modal_widgets/sourcesmanagerdialog.h ui/modal_widgets/sourcesmanagerdialog.cpp
)

target_include_directories(audio-lib-super PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/module_panels
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/qt_items
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/modal_widgets
)

target_link_libraries(audio-lib-super PRIVATE Qt::Widgets Qt::Svg Qt::Multimedia)

include(GNUInstallDirs)
install(TARGETS audio-lib-super
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)


qt_finalize_executable(audio-lib-super)
