// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "PanoramioPlugin.h"

#include "PanoramioModel.h"

#include "MarbleWidget.h"

using namespace Marble;

PanoramioPlugin::PanoramioPlugin(const MarbleModel *marbleModel)
    : AbstractDataPlugin(marbleModel)
{
}

QString Marble::PanoramioPlugin::nameId() const
{
    return QStringLiteral("panoramio");
}

void PanoramioPlugin::initialize()
{
    setModel(new PanoramioModel(marbleModel(), this));
    setNumberOfItems(numberOfImagesPerFetch);
}

QString PanoramioPlugin::name() const
{
    return tr("Panoramio Photos");
}

QString PanoramioPlugin::guiString() const
{
    return tr("&Panoramio");
}

QString PanoramioPlugin::description() const
{
    return tr("Automatically downloads images from around the world in preference to their popularity");
}

QIcon PanoramioPlugin::icon() const
{
    return QIcon(QStringLiteral(":/icons/panoramio.png"));
}

QString Marble::PanoramioPlugin::version() const
{
    return QStringLiteral("0.1");
}

QString PanoramioPlugin::copyrightYears() const
{
    return QStringLiteral("2009, 2014");
}

QList<PluginAuthor> PanoramioPlugin::pluginAuthors() const
{
    return QList<PluginAuthor>() << PluginAuthor(QStringLiteral("Bastian Holst"), QStringLiteral("bastianholst@gmx.de"));
}

bool PanoramioPlugin::eventFilter(QObject *object, QEvent *event)
{
    if (isInitialized()) {
        Q_ASSERT(dynamic_cast<PanoramioModel *>(model()) != 0);

        PanoramioModel *photoPluginModel = static_cast<PanoramioModel *>(model());
        MarbleWidget *widget = dynamic_cast<MarbleWidget *>(object);
        if (widget) {
            photoPluginModel->setMarbleWidget(widget);
        }
    }

    return AbstractDataPlugin::eventFilter(object, event);
}

#include "moc_PanoramioPlugin.cpp"
