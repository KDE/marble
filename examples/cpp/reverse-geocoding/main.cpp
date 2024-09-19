// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include <QApplication>
#include <QDebug>

#include <marble/MarbleModel.h>
#include <marble/MarbleWidget.h>
#include <marble/ReverseGeocodingRunnerManager.h>

using namespace Marble;

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    MarbleModel model;
    ReverseGeocodingRunnerManager manager(&model);

    GeoDataCoordinates position(-0.15845, 51.52380, 0.0, GeoDataCoordinates::Degree);
    qDebug() << position.toString() << "is" << manager.searchReverseGeocoding(position);
}
