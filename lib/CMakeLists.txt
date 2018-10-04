#
# Copyright (C) 2013, 2016 Bastian Bloessl <bloessl@ccs-labs.org>
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
include (CheckCCompilerFlag)
CHECK_C_COMPILER_FLAG ("-msse2" SSE2_SUPPORTED)

if(SSE2_SUPPORTED)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse2")
    add_definitions(-DIEEE80211_MSSE2)
endif(SSE2_SUPPORTED)

########################################################################
# Setup library
########################################################################
list(APPEND gr_ieee802_11_sources
    chunks_to_symbols_impl.cc
    constellations_impl.cc
    decode_mac.cc
    equalizer/base.cc
    equalizer/comb.cc
    equalizer/ls.cc
    equalizer/lms.cc
    equalizer/sta.cc
    ether_encap_impl.cc
    frame_equalizer_impl.cc
    mac.cc
    mapper.cc
    parse_mac.cc
    signal_field_impl.cc
    sync_long.cc
    sync_short.cc
    utils.cc
    viterbi_decoder/base.cc
)

# use SSE2 optimized viterbi implementation if SSE2 is enabled
if(SSE2_SUPPORTED)
    list(APPEND gr_ieee802_11_sources
        viterbi_decoder/viterbi_decoder_x86.cc
    )
else()
    list(APPEND gr_ieee802_11_sources
        viterbi_decoder/viterbi_decoder_generic.cc
    )
endif(SSE2_SUPPORTED)

list(APPEND gr_ieee802_11_libs
    ${Boost_LIBRARIES}
    ${GNURADIO_ALL_LIBRARIES}
)

add_library(gnuradio-ieee802_11 SHARED ${gr_ieee802_11_sources})
target_link_libraries(gnuradio-ieee802_11 ${gr_ieee802_11_libs})
set_target_properties(gnuradio-ieee802_11 PROPERTIES DEFINE_SYMBOL "gnuradio_ieee802_11_EXPORTS")

########################################################################
# Install built library files
########################################################################
install(TARGETS gnuradio-ieee802_11
    LIBRARY DESTINATION lib${LIB_SUFFIX} # .so/.dylib file
    ARCHIVE DESTINATION lib${LIB_SUFFIX} # .lib file
    RUNTIME DESTINATION bin              # .dll file
)
