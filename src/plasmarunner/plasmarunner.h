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

class PlasmaRunner : public Plasma::AbstractRunner
{
    Q_OBJECT

public:
#if KRUNNER_VERSION >= QT_VERSION_CHECK(5, 77, 0)
    PlasmaRunner(QObject *parent, const KPluginMetaData &pluginMetaData, const QVariantList &args);
#else
    PlasmaRunner(QObject *parent, const QVariantList &args);
#endif

public: // Plasma::AbstractRunner API
    void match(Plasma::RunnerContext &context) override;
    void run(const Plasma::RunnerContext &context, const Plasma::QueryMatch &match) override;

private:
    void collectMatches(QList<Plasma::QueryMatch> &matches, const QString &query, const GeoDataFolder *folder);
};

}

#endif
