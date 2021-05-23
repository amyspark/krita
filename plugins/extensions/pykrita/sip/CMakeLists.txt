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

    find_file(sip_generate "sip-generate.py" PATHS ${CMAKE_MODULE_PATH})
    find_file(pyproject_toml "pyproject.toml.in" PATHS ${CMAKE_MODULE_PATH})

    set(module_name "kritamod")
    set(module_srcs "${CMAKE_CURRENT_SOURCE_DIR}/krita")
    set(PYTHON_SITE_PACKAGES_INSTALL_DIR ${LIB_INSTALL_DIR}/krita-python-libs)
    configure_file(
        ${pyproject_toml}
        ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
    )
    set(_tmp_build_dir "${CMAKE_CURRENT_BINARY_DIR}/_tmp/")
    set(_sip_base "${_tmp_build_dir}/krita/")
    set(_sip_outputs
        ${_sip_base_libraries}
        ${_sip_base}/sipkritapart0.cpp
        ${_sip_base}/sipkritapart1.cpp
        ${_sip_base}/sipkritapart2.cpp
        ${_sip_base}/sipkritapart3.cpp
        ${_sip_base}/sipkritapart4.cpp
        ${_sip_base}/sipkritapart5.cpp
        ${_sip_base}/sipkritapart6.cpp
        ${_sip_base}/sipkritapart7.cpp
    )
    add_custom_command(
        COMMAND
            ${PYTHON_EXECUTABLE}
            ${sip_generate}
            --build-dir ${_tmp_build_dir}
            --target-dir ${PYTHON_SITE_PACKAGES_INSTALL_DIR}
            --concatenate 8
            --protected-is-public
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS
            ${CMAKE_CURRENT_BINARY_DIR}/pyproject.toml
        OUTPUT
            ${_sip_outputs}
    )
    add_library(python_module_Pykrita_krita SHARED ${_sip_outputs})
    target_include_directories(python_module_Pykrita_krita PRIVATE ${_tmp_build_dir})
    target_link_libraries(python_module_Pykrita_krita ${PYTHON_LIBRARY})
    target_link_libraries(python_module_Pykrita_krita kritalibkis kritaui kritaimage)
    set_target_properties(python_module_Pykrita_krita PROPERTIES PREFIX "" OUTPUT_NAME krita)
    set_target_properties(python_module_Pykrita_krita PROPERTIES C_STANDARD 99)
    set_target_properties(python_module_Pykrita_krita PROPERTIES C_STANDARD_REQUIRED ON)

    if (MINGW)
        target_compile_definitions(python_module_Pykrita_krita PRIVATE _hypot=hypot)
    else()
        target_compile_definitions(python_module_Pykrita_krita PRIVATE SIP_PROTECTED_IS_PUBLIC)
        target_compile_definitions(python_module_Pykrita_krita PRIVATE protected=public)
    endif (MINGW)

    if (WIN32)
        set_target_properties(python_module_Pykrita_krita PROPERTIES SUFFIX ".pyd")
    endif ()

    install(TARGETS python_module_Pykrita_krita DESTINATION "${PYTHON_SITE_PACKAGES_INSTALL_DIR}/PyKrita")
ENDIF()
