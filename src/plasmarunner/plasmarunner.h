// SPDX-FileCopyrightText: 2011 Friedrich W. H. Kossebau <kossebau@kde.org>
// SPDX-License-Identifier: LGPL-2.1-or-later

#ifndef MARBLE_PLASMARUNNER_H
#define MARBLE_PLASMARUNNER_H

// Plasma
#include <KRunner/AbstractRunner>
#include <krunner_version.h>

namespace Marble
{
class GeoDataFolder;

class PlasmaRunner : public KRunner::AbstractRunner
{
    Q_OBJECT

public:
    PlasmaRunner(QObject *parent, const KPluginMetaData &pluginMetaData);

public: // Plasma::AbstractRunner API
    void match(KRunner::RunnerContext &context) override;
    void run(const KRunner::RunnerContext &context, const KRunner::QueryMatch &match) override;

private:
    void collectMatches(QList<KRunner::QueryMatch> &matches, const QString &query, const GeoDataFolder *folder);
};

}

#endif
