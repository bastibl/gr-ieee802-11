#
# Copyright (C) 2013 Bastian Bloessl <bloessl@ccs-labs.org>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

########################################################################
# Include swig generation macros
########################################################################
include(GrPython)
find_package(SWIG)
find_package(PythonLibs)
if(NOT SWIG_FOUND OR NOT PYTHONLIBS_FOUND)
    return()
endif()
include(GrSwig)

########################################################################
# Setup swig generation
########################################################################
set(GR_SWIG_INCLUDE_DIRS
    ${GR_IEEE802_11_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

foreach(incdir ${GNURADIO_RUNTIME_INCLUDE_DIRS})
    list(APPEND GR_SWIG_INCLUDE_DIRS ${incdir}/gnuradio/swig)
endforeach(incdir)

foreach(incdir ${GRUEL_INCLUDE_DIRS})
    list(APPEND GR_SWIG_INCLUDE_DIRS ${incdir}/gruel/swig)
endforeach(incdir)

set(GR_SWIG_LIBRARIES gnuradio-ieee802_11)
set(GR_SWIG_DOC_FILE ${CMAKE_CURRENT_BINARY_DIR}/ieee802_11_swig_doc.i)
set(GR_SWIG_DOC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../include/gnuradio/ieee802_11)

GR_SWIG_MAKE(ieee802_11_swig ieee802_11_swig.i)

########################################################################
# Install the build swig module
########################################################################
GR_SWIG_INSTALL(
    TARGETS ieee802_11_swig
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ieee802_11
    COMPONENT "ieee802_11_python"
)

########################################################################
# Install swig .i files for development
########################################################################
list(APPEND swig_sources
    ieee802_11_swig.i
    ieee802_11_factory.i
    ieee802_11_ofdm_decode_mac.i
    ieee802_11_ofdm_decode_signal.i
    ieee802_11_ofdm_equalize_symbols.i
    ieee802_11_ofdm_parse_mac.i
    ieee802_11_ofdm_sync_long.i
    ieee802_11_ofdm_sync_short.i
    ${CMAKE_CURRENT_BINARY_DIR}/ieee802_11_swig_doc.i
)

install(
    FILES ${swig_sources}
    DESTINATION ${GR_INCLUDE_DIR}/gnuradio/ieee802_11/swig
    COMPONENT "ieee802_11_swig"
)
