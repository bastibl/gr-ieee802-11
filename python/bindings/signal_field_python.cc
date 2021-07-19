/*
 * Copyright 2021 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(signal_field.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(704b5399c8f91c472b1826c74dee0a5a)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <ieee802_11/signal_field.h>
// pydoc.h is automatically generated in the build directory
#include <signal_field_pydoc.h>

void bind_signal_field(py::module& m)
{

    using signal_field    = ::gr::ieee802_11::signal_field;


    py::class_<signal_field, gr::digital::packet_header_default,
        std::shared_ptr<signal_field>>(m, "signal_field", D(signal_field))

        .def(py::init(&signal_field::make),
           D(signal_field,make)
        )
        




        
        .def("base",&signal_field::base,       
            D(signal_field,base)
        )


        
        .def("formatter",&signal_field::formatter,       
            D(signal_field,formatter)
        )

        ;




}







