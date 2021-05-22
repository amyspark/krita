include(SIPMacros)

message( ${SIP_VERSION} " - The version of SIP found expressed as a 6 digit hex number suitable for comparison as a string.")
message( ${SIP_VERSION_STR} " - The version of SIP found as a human readable string.")
message( ${SIP_EXECUTABLE} " - Path and filename of the SIP command line executable.")
message( ${SIP_DEFAULT_SIP_DIR} " - default SIP dir" )
IF(${SIP_VERSION_STR} VERSION_LESS 5)
    message( ${SIP_INCLUDE_DIR} " - Directory holding the SIP C++ header file.")

    set(SIP_INCLUDES
        ${SIP_DEFAULT_SIP_DIR}
        ${PYQT5_SIP_DIR}
        ${PYQT_SIP_DIR_OVERRIDE}
        ./krita)

    set(SIP_CONCAT_PARTS 1)
    set(SIP_TAGS ALL WS_X11 ${PYQT5_VERSION_TAG})
    set(SIP_EXTRA_OPTIONS -g -o -x PyKDE_QVector)
    IF (PYQT5_SIP_NAME)
        set(SIP_EXTRA_OPTIONS ${SIP_EXTRA_OPTIONS} -n ${PYQT5_SIP_NAME})
    ENDIF (PYQT5_SIP_NAME)

    set(PYTHON_SITE_PACKAGES_INSTALL_DIR ${LIB_INSTALL_DIR}/krita-python-libs)
    file(GLOB PYKRITA_KRITA_sip_files ./krita/*.sip)
    set(SIP_EXTRA_FILES_DEPEND ${PYKRITA_KRITA_sip_files})
    add_sip_python_module(PyKrita.krita ./krita/kritamod.sip kritalibkis kritaui kritaimage kritalibbrush)

    if (ENABLE_PYTHON_2)
        # Add an init file to turn it into a valid py2 module.
        # Otherwise PyKrita cannot be loaded.
        install(FILES
            ./__init__.py
            DESTINATION ${PYTHON_SITE_PACKAGES_INSTALL_DIR}/PyKrita)
    endif (ENABLE_PYTHON_2)
else()
    message(STATUS "Using SIP 5+ for packaging PyKrita.")

    set(PYKRITA_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/krita)
    
    set(PYKRITA_INCLUDES "")
    set(PYKRITA_LIBRARIES "")
    foreach(_lib kritalibkis kritaui kritaimage)
        get_target_property(_lib_includes ${_lib} INCLUDE_DIRECTORIES)
        foreach(_libdir ${_lib_includes})
            string(APPEND PYKRITA_INCLUDES "\"${_libdir}\", ")
        endforeach()
        get_target_property(_lib_includes ${_lib} SOURCE_DIR)
        string(APPEND PYKRITA_INCLUDES "\"${_lib_includes}\", ")
        string(APPEND PYKRITA_LIBRARIES "\"${_lib}\", ")
    endforeach()
    foreach(_libdir ${Qt5Core_INCLUDE_DIRS})
        string(APPEND PYKRITA_INCLUDES "\"${_libdir}\", ")
    endforeach()

    set(PYKRITA_INCLUDES "[ ${PYKRITA_INCLUDES} ]")
    set(PYKRITA_LIBRARIES "[ ${PYKRITA_LIBRARIES} ]")
    set(PYTHON_SITE_PACKAGES_INSTALL_DIR ${LIB_INSTALL_DIR}/krita-python-libs)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml
        ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
    )
    add_custom_target(
        PyKrita.krita
        ALL
        COMMAND
            ${SIP_EXECUTABLE}
            --verbose
            --build-dir ${CMAKE_CURRENT_BINARY_DIR}
            --target-dir ${PYTHON_SITE_PACKAGES_INSTALL_DIR}
            --concatenate 1
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        BYPRODUCTS
            krita.pyd
    )
ENDIF()
