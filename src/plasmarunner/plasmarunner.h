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


#ifndef PLASMARUNNER_H
#define PLASMARUNNER_H

// Plasma
#include <Plasma/AbstractRunner>


namespace Marble
{

class PlasmaRunner : public Plasma::AbstractRunner
{
    Q_OBJECT

public:
    PlasmaRunner(QObject *parent, const QVariantList &args);

public: // Plasma::AbstractRunner API
    void match(Plasma::RunnerContext &context);
    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match);
};

}

#endif

