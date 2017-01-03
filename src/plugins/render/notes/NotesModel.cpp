//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Spencer Brown <spencerbrown991@gmail.com>
//

#include "NotesModel.h"
#include "NotesItem.h"

#include "MarbleGlobal.h"
#include "MarbleModel.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleDebug.h"
#include "Planet.h"

#include <QString>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <QUrlQuery>

using namespace Marble;

NotesModel::NotesModel( const MarbleModel *marbleModel, QObject *parent  )
    : AbstractDataPluginModel( "Notes", marbleModel, parent )
{
}

void NotesModel::getAdditionalItems( const GeoDataLatLonAltBox& box, qint32 number )
{
    double left = box.west( GeoDataCoordinates::Degree );
    double bottom = box.south( GeoDataCoordinates::Degree );
    double right = box.east( GeoDataCoordinates::Degree );
    double top = box.north( GeoDataCoordinates::Degree );

    QString bboxValue;
    bboxValue.append(QString::number( left )).append(",").append(QString::number( bottom )).append(",").append(QString::number( right )).append(",").append(QString::number( top ));

    QUrl osmNotesApiUrl("http://api.openstreetmap.org/api/0.6/notes.json");
    QUrlQuery urlQuery;
    urlQuery.addQueryItem( "bbox", bboxValue );
    urlQuery.addQueryItem ("limit", QString::number( number ));
    osmNotesApiUrl.setQuery(urlQuery);

    downloadDescriptionFile( osmNotesApiUrl );
}

void NotesModel::parseFile( const QByteArray& file )
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file);
    QJsonValue features = jsonDoc.object().value(QStringLiteral("features"));

    if (features.isArray()) {
        QList<AbstractDataPluginItem*> items;

        QJsonArray jsonArray = features.toArray();
        for (auto jsonRef : jsonArray) {

            QJsonObject jsonObj = jsonRef.toObject();
            QJsonObject geometry = jsonObj.value(QStringLiteral("geometry")).toObject();
            QJsonArray coordinates = geometry.value(QStringLiteral("coordinates")).toArray();
            double longitude = coordinates.at(0).toDouble();
            double latitude = coordinates.at(1).toDouble();

            GeoDataCoordinates coordinateset( longitude, latitude, 0.0, GeoDataCoordinates::Degree );
            QString id = QString::number(jsonObj.value(QStringLiteral("properties")).toObject().value(QStringLiteral("id")).toInt());
            qDebug() << id;

            NotesItem *item = new NotesItem (this);
            item->setId( id );
            item->setCoordinate( coordinateset );

            items << item;
        }

        addItemsToList( items );
    }
}

#include "moc_NotesModel.cpp"
