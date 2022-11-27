// SPDX-FileCopyrightText: 2011 Friedrich W. H. Kossebau <kossebau@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#include "plasmarunner.h"

#if KRUNNER_VERSION >= QT_VERSION_CHECK(5, 72, 0)
using namespace Marble;
K_PLUGIN_CLASS_WITH_JSON(PlasmaRunner, "plasma-runner-marble.json")
#else
K_EXPORT_PLASMA_RUNNER(marble, Marble::PlasmaRunner)
#endif

#include "main.moc"
