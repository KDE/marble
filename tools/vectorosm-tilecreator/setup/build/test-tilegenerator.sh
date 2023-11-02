#!/bin/bash
# SPDX-License-Identifier: BSD-3-Clause
# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>

# just needs to be set to something in order to be able to use standalone test mode
export TIREX_BACKEND_SOCKET_FILENO=42

# test if the previously built tile generator can successfully generate a metatile
cd
/output/usr/bin/marble-vectorosm-tirex-backend -x 68369 -y 46578 -z 17 -c /var/lib/tirex/cache
find output
ls -lah output/17/16/11/181/31/18.meta
grep Randa output/17/16/11/181/31/18.meta
