find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_TETRA_DMO gnuradio-TETRA_DMO)

FIND_PATH(
    GR_TETRA_DMO_INCLUDE_DIRS
    NAMES gnuradio/TETRA_DMO/api.h
    HINTS $ENV{TETRA_DMO_DIR}/include
        ${PC_TETRA_DMO_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_TETRA_DMO_LIBRARIES
    NAMES gnuradio-TETRA_DMO
    HINTS $ENV{TETRA_DMO_DIR}/lib
        ${PC_TETRA_DMO_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
          )

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-TETRA_DMOTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_TETRA_DMO DEFAULT_MSG GR_TETRA_DMO_LIBRARIES GR_TETRA_DMO_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_TETRA_DMO_LIBRARIES GR_TETRA_DMO_INCLUDE_DIRS)
