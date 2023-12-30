cmake_minimum_required(VERSION 3.16)

project(EncryptorApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.2 REQUIRED COMPONENTS Quick)


qt_add_executable(appEncryptorApp
    main.cpp
    encryptor.h encryptor.cpp
    filemanager.h filemanager.cpp
)

qt_add_qml_module(appEncryptorApp
    URI EncryptorApp
    VERSION 1.0
    QML_FILES Main.qml PassField.qml FileBrowser.qml CustomButton.qml
    SOURCES encryptor.h encryptor.cpp filemanager.h filemanager.cpp

)

set_target_properties(appEncryptorApp PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appEncryptorApp
    PRIVATE Qt6::Quick
)

install(TARGETS appEncryptorApp
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
