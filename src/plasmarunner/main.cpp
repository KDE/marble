// Copyright 2011 Friedrich W. H. Kossebau <kossebau@kde.org>
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this program. If not, see <http://www.gnu.org/licenses/>.

#include "plasmarunner.h"

#if KRUNNER_VERSION >= QT_VERSION_CHECK(5, 72, 0)
using namespace Marble;
K_EXPORT_PLASMA_RUNNER_WITH_JSON(PlasmaRunner, "plasma-runner-marble.json")
#else
K_EXPORT_PLASMA_RUNNER(marble, Marble::PlasmaRunner)
#endif

#include "main.moc"
