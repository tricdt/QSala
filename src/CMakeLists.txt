list(APPEND HEADERS
    common/QskGlobal.h
)
list(APPEND SOURCES
    controls/QskWindow.cpp
)
list(APPEND HEADERS
    controls/QskWindow.h
)
if(ENABLE_PINYIN)
    list(APPEND HEADERS inputpanel/QskPinyinTextPredictor.h)
    list(APPEND SOURCES inputpanel/QskPinyinTextPredictor.cpp)
endif()

if(ENABLE_HUNSPELL)
    list(APPEND HEADERS inputpanel/QskHunspellTextPredictor.h)
    list(APPEND SOURCES inputpanel/QskHunspellTextPredictor.cpp)
endif()

set(target qskinny)

if(BUILD_QSKDLL)
    qsk_add_library(${target} SHARED ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
    target_compile_definitions(${target} PUBLIC QSK_DLL)
    set_target_properties(${target} PROPERTIES DEFINE_SYMBOL QSK_MAKEDLL)
else()
    qsk_add_library(${target} STATIC ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS})
endif()

if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
    qsk_add_shaders(${target} "qskshaders" FILES ${SHADERS} OUTPUT_TARGETS shader_target)
endif()

target_include_directories(${target} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/common>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/controls>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/dialogs>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/graphic>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/inputpanel>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/layouts>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/nodes>)

target_include_directories(${target}
    INTERFACE
    $<INSTALL_INTERFACE:${QSK_INSTALL_HEADERS}>)

target_link_libraries(${target}
    PUBLIC Qt::Core Qt::CorePrivate Qt::Quick Qt::QuickPrivate)

if(QT_VERSION_MAJOR VERSION_GREATER 5)
    target_link_libraries(${target} PUBLIC Qt::OpenGL Qt::OpenGLPrivate)
endif()

if(ENABLE_HUNSPELL)
    target_link_libraries(${target} PRIVATE hunspell)
endif()

if(ENABLE_PINYIN)
    target_compile_definitions(${target} PRIVATE PINYIN)
    target_link_libraries(${target} PRIVATE pinyin Fcitx5::Utils)

    target_include_directories(${target}
        PUBLIC $<BUILD_INTERFACE:${QSK_SOURCE_DIR}/inputcontext>)
endif()

set_target_properties(${target} PROPERTIES FOLDER libs)

# TODO hack for standalone qvg2svg
set_target_properties(${target}
    PROPERTIES AUTOGEN_BUILD_DIR ${CMAKE_CURRENT_BINARY_DIR}/qskinny_autogen)

list(TRANSFORM HEADERS PREPEND "${CMAKE_CURRENT_LIST_DIR}/")
set_target_properties(${target} PROPERTIES PUBLIC_HEADER "${HEADERS}")
set_target_properties(${target} PROPERTIES EXPORT_NAME ${PROJECT_NAME})
set_target_properties(${target} PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR})

# packaging
set(PACKAGE_NAME ${PROJECT_NAME})

set(exported_targets ${target})
if(NOT BUILD_QSKDLL)
    list(APPEND exported_targets ${shader_target})
endif()


# setup destination file paths for qskinny target
install(TARGETS ${exported_targets} EXPORT ${PACKAGE_NAME}Targets
    LIBRARY DESTINATION ${QSK_INSTALL_LIBS}
    ARCHIVE DESTINATION ${QSK_INSTALL_LIBS}
    RUNTIME DESTINATION ${QSK_INSTALL_LIBS}
    INCLUDES DESTINATION ${QSK_INSTALL_HEADERS}
    PUBLIC_HEADER DESTINATION ${QSK_INSTALL_HEADERS})

# export QSkinnyTargets namespace
export(EXPORT ${PACKAGE_NAME}Targets
    NAMESPACE ${PACKAGE_NAMESPACE})

# install QSkinnyTargets under QSkinny namespace
install(EXPORT ${PACKAGE_NAME}Targets
    FILE
    ${PACKAGE_NAME}Targets.cmake
    NAMESPACE
    ${PACKAGE_NAMESPACE}
    DESTINATION
    ${PACKAGE_LOCATION})

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES $<TARGET_PDB_FILE:${target}>
        DESTINATION
        ${QSK_INSTALL_LIBS} OPTIONAL)
endif()
