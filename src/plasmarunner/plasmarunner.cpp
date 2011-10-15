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

// Marble
#include <GeoDataCoordinates.h>
// KDE
#include <KProcess>
#include <KIcon>
#include <KLocale>


namespace Marble
{

PlasmaRunner::PlasmaRunner(QObject *parent, const QVariantList &args)
  : AbstractRunner(parent, args)
{
    setIgnoredTypes(Plasma::RunnerContext::NetworkLocation |
                    Plasma::RunnerContext::FileSystem |
                    Plasma::RunnerContext::Help);

    const Plasma::RunnerSyntax syntax(QLatin1String(":q:"),
                                      i18n("Shows the coordinates :q: in OpenStreetMap with Marble."));
    addSyntax(syntax);
}

void PlasmaRunner::match(Plasma::RunnerContext &context)
{
    const QString query = context.query();

    bool success = false;
    // TODO: how to estimate that input is in Degree, not Radian?
    GeoDataCoordinates coordinates = GeoDataCoordinates::fromString(query, success);

    if (success) {
        const QVariant coordinatesData = QVariantList()
            << QVariant(coordinates.longitude(GeoDataCoordinates::Degree))
            << QVariant(coordinates.latitude(GeoDataCoordinates::Degree));

        Plasma::QueryMatch match(this);
        match.setIcon(KIcon(QLatin1String("marble")));
        match.setText(i18n("Show the coordinates %1 in OpenStreetMap with Marble", query));
        match.setData(coordinatesData);
        match.setId(query);
        match.setRelevance(1.0);
        match.setType(Plasma::QueryMatch::ExactMatch);

        context.addMatch(query, match);
    }
}

void PlasmaRunner::run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match)
{
    Q_UNUSED(context)

    const QVariantList data = match.data().toList();

    const QString latLon =
        QString::fromUtf8("%L1").arg(data.at(1).toReal()) +
        QString::fromUtf8(" %L1").arg(data.at(0).toReal());

    const QStringList parameters = QStringList()
        << QLatin1String( "--latlon" )
        << latLon
        << QLatin1String( "--map" )
        << QLatin1String( "earth/openstreetmap/openstreetmap.dgml" );
    KProcess::startDetached( QLatin1String("marble"), parameters );
}

}
