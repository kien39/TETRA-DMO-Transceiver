# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT TETRA_DMO_sources)
    message(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT TETRA_DMO_sources)

########################################################################
# Check for pygccxml
########################################################################
gr_python_check_module_raw("pygccxml" "import pygccxml" PYGCCXML_FOUND)

include(GrPybind)

########################################################################
# Python Bindings
########################################################################

list(APPEND TETRA_DMO_python_files
    MAC_DECODER_python.cc python_bindings.cc)

gr_pybind_make_oot(TETRA_DMO ../../.. gr::TETRA_DMO "${TETRA_DMO_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(
    TARGET TETRA_DMO_python
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:TETRA_DMO_python>
            ${PROJECT_BINARY_DIR}/test_modules/gnuradio/TETRA_DMO/)

install(
    TARGETS TETRA_DMO_python
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/TETRA_DMO
    COMPONENT pythonapi)
