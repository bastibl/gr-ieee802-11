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
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

list(APPEND gr_ieee802_11_sources
    ofdm_decode_mac.cc
    ofdm_decode_signal.cc
    ofdm_equalize_symbols.cc
    ofdm_parse_mac.cc
    ofdm_sync_long.cc
    ofdm_sync_short.cc
)

list(APPEND gr_ieee802_11_libs
    ${Boost_LIBRARIES}
    ${GRUEL_LIBRARIES}
    ${GNURADIO_CORE_LIBRARIES}
)

add_library(gnuradio-ieee802_11 SHARED ${gr_ieee802_11_sources})
target_link_libraries(gnuradio-ieee802_11 ${gr_ieee802_11_libs} itpp gnuradio-fft gnuradio-filter)
set_target_properties(gnuradio-ieee802_11 PROPERTIES DEFINE_SYMBOL "gnuradio_ieee802_11_EXPORTS")

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-ieee802_11
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)
