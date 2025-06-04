# Copyright 2023 Ryan Volz
#
# This file is part of gr-ieee802_11
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

install(
    FILES wifi_loopback.grc
          wifi_phy_hier.grc
          wifi_rx.grc
          wifi_transceiver.grc
          wifi_tx.grc
    DESTINATION ${GR_PKG_DATA_DIR}/examples)
