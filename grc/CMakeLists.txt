file(GLOB xml_files "*.block.yml")

install(FILES ${xml_files} DESTINATION share/gnuradio/grc/blocks )
