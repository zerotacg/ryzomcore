cmake_minimum_required(VERSION 3.27)

project(Studio VERSION 6.0 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)


add_library(qtpropertybrowser STATIC)
add_library(QtPropertyBrowser::QtPropertyBrowser ALIAS qtpropertybrowser)
target_sources(qtpropertybrowser
        PRIVATE
        src/qtpropertybrowser.cpp
        src/qtpropertymanager.cpp
        src/qteditorfactory.cpp
        src/qtvariantproperty.cpp
        src/qttreepropertybrowser.cpp
        src/qtbuttonpropertybrowser.cpp
        src/qtgroupboxpropertybrowser.cpp
        src/qtpropertybrowserutils.cpp

        src/qtpropertybrowser.qrc
        PUBLIC
        FILE_SET HEADERS
        FILES
        src/QtAbstractEditorFactoryBase
        src/QtAbstractPropertyBrowser
        src/QtAbstractPropertyManager
        src/QtBoolPropertyManager
        src/QtBrowserItem
        src/QtButtonPropertyBrowser
        src/QtCharEditorFactory
        src/QtCharPropertyManager
        src/QtCheckBoxFactory
        src/QtColorEditorFactory
        src/QtColorPropertyManager
        src/QtCursorEditorFactory
        src/QtCursorPropertyManager
        src/QtDateEditFactory
        src/QtDatePropertyManager
        src/QtDateTimeEditFactory
        src/QtDateTimePropertyManager
        src/QtDoublePropertyManager
        src/QtDoubleSpinBoxFactory
        src/QtEnumEditorFactory
        src/QtEnumPropertyManager
        src/QtFlagPropertyManager
        src/QtFontEditorFactory
        src/QtFontPropertyManager
        src/QtGroupBoxPropertyBrowser
        src/QtGroupPropertyManager
        src/QtIntPropertyManager
        src/QtKeySequenceEditorFactory
        src/QtKeySequencePropertyManager
        src/QtLineEditFactory
        src/QtLocalePropertyManager
        src/QtPointFPropertyManager
        src/QtPointPropertyManager
        src/QtProperty
        src/QtRectFPropertyManager
        src/QtRectPropertyManager
        src/QtScrollBarFactory
        src/QtSizeFPropertyManager
        src/QtSizePolicyPropertyManager
        src/QtSizePropertyManager
        src/QtSliderFactory
        src/QtSpinBoxFactory
        src/QtStringPropertyManager
        src/QtTimeEditFactory
        src/QtTimePropertyManager
        src/QtTreePropertyBrowser
        src/QtVariantEditorFactory
        src/QtVariantProperty
        src/QtVariantPropertyManager
        src/qtbuttonpropertybrowser.h
        src/qteditorfactory.h
        src/qtgroupboxpropertybrowser.h
        src/qtpropertybrowser.h
        src/qtpropertybrowserutils_p.h
        src/qtpropertymanager.h
        src/qttreepropertybrowser.h
        src/qtvariantproperty.h
)

target_include_directories(qtpropertybrowser PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(qtpropertybrowser PRIVATE Qt::Widgets Qt::Core Qt::Gui)
