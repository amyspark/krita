#
# From https://github.com/ampl/gsl. which adds CMake support to gsl
#
if (UNIX)
SET(EXTPREFIX_gsl "${EXTPREFIX}" )
ExternalProject_Add( ext_gsl

    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL http://files.kde.org/krita/build/dependencies/gsl-2.3.0.tar.gz
    URL_MD5 7e0478f7c5e62696fef480b9a46f708c

    PATCH_COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/gsl-android.patch
                COMMAND ${PATCH_COMMAND} -p1 -i ${CMAKE_CURRENT_SOURCE_DIR}/gsl-2.3_clang12.patch
    INSTALL_DIR ${EXTPREFIX_gsl}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_gsl} -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE} ${GLOBAL_PROFILE} -DBUILD_SHARED_LIBS=ON -DBUILD_TESTING=OFF
    UPDATE_COMMAND ""
)
else()
SET(EXTPREFIX_gsl "${EXTPREFIX}" )
ExternalProject_Add( ext_gsl

    DOWNLOAD_DIR ${EXTERNALS_DOWNLOAD_DIR}
    URL http://files.kde.org/krita/build/dependencies/gsl-2.3.0.tar.gz
    URL_MD5 7e0478f7c5e62696fef480b9a46f708c

    INSTALL_DIR ${EXTPREFIX_gsl}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${EXTPREFIX_gsl} -DCMAKE_BUILD_TYPE=${GLOBAL_BUILD_TYPE} ${GLOBAL_PROFILE} -DBUILD_TESTING=OFF
    UPDATE_COMMAND ""
)

endif()
