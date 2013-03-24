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
# Setup python install
########################################################################
include(GrPython)

list(APPEND python_sources
    __init__.py
)

GR_PYTHON_INSTALL(
    FILES ${python_sources}
    DESTINATION ${GR_PYTHON_DIR}/gnuradio/ieee802_11
    COMPONENT "ieee802_11_python"
)

########################################################################
# Handle the unit tests
########################################################################
include(GrTest)

#set(GR_TEST_TARGET_DEPS gnuradio-ieee802-15-4)
#set(GR_TEST_PYTHON_DIRS ${CMAKE_BINARY_DIR}/swig)
#GR_ADD_TEST(qa_howto ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/qa_howto.py)
