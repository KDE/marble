// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2017 Spencer Brown <spencerbrown991@gmail.com>
//

#include "NotesModel.h"
#include "NotesItem.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleModel.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>
#include <QUrl>

#include <QUrlQuery>

using namespace Marble;

NotesModel::NotesModel(const MarbleModel *marbleModel, QObject *parent)
    : AbstractDataPluginModel(QStringLiteral("Notes"), marbleModel, parent)
{
}

void NotesModel::getAdditionalItems(const GeoDataLatLonAltBox &box, qint32 number)
{
    double left = box.west(GeoDataCoordinates::Degree);
    double bottom = box.south(GeoDataCoordinates::Degree);
    double right = box.east(GeoDataCoordinates::Degree);
    double top = box.north(GeoDataCoordinates::Degree);

    QString bboxValue;
    bboxValue.append(QString::number(left))
        .append(QStringLiteral(","))
        .append(QString::number(bottom))
        .append(QStringLiteral(","))
        .append(QString::number(right))
        .append(QStringLiteral(","))
        .append(QString::number(top));

    QUrl osmNotesApiUrl(QStringLiteral("https://api.openstreetmap.org/api/0.6/notes.json"));
    QUrlQuery urlQuery;
    urlQuery.addQueryItem(QStringLiteral("bbox"), bboxValue);
    urlQuery.addQueryItem(QStringLiteral("limit"), QString::number(number));
    osmNotesApiUrl.setQuery(urlQuery);

    downloadDescriptionFile(osmNotesApiUrl);
}

void NotesModel::parseFile(const QByteArray &file)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(file);
    QJsonValue features = jsonDoc.object().value(QStringLiteral("features"));

    if (features.isArray()) {
        QList<AbstractDataPluginItem *> items;

        QJsonArray jsonArray = features.toArray();
        for (auto const jsonRef : std::as_const(jsonArray)) {
            QJsonObject jsonObj = jsonRef.toObject();
            QJsonObject geometry = jsonObj.value(QStringLiteral("geometry")).toObject();
            QJsonArray coordinates = geometry.value(QStringLiteral("coordinates")).toArray();
            double lon = coordinates.at(0).toDouble();
            double lat = coordinates.at(1).toDouble();

            QJsonObject noteProperties = jsonObj.value(QStringLiteral("properties")).toObject();
            QJsonArray noteComments = noteProperties.value(QStringLiteral("comments")).toArray();

            QString id = QString::number(noteProperties.value(QStringLiteral("id")).toInt());

            QDateTime dateCreated = QDateTime::fromString(noteProperties.value(QStringLiteral("date_created")).toString(), Qt::ISODate);
            QDateTime dateClosed = QDateTime::fromString(noteProperties.value(QStringLiteral("closed_at")).toString(), Qt::ISODate);
            QString noteStatus = noteProperties.value(QStringLiteral("status")).toString();

            auto item = new NotesItem(this);
            item->setId(id);
            item->setCoordinate(GeoDataCoordinates(lon, lat, 0.0, GeoDataCoordinates::Degree));
            item->setDateCreated(dateCreated);
            item->setNoteStatus(noteStatus);
            item->setDateClosed(dateClosed);

            for (auto const commentRef : std::as_const(noteComments)) {
                QJsonObject commentObj = commentRef.toObject();
                QDateTime date = QDateTime::fromString(commentObj.value(QStringLiteral("date")).toString(), Qt::ISODate);
                QString user = commentObj.value(QStringLiteral("user")).toString();
                QString text = commentObj.value(QStringLiteral("text")).toString();
                int uid = commentObj.value(QStringLiteral("uid")).toInt();
                Comment comment(date, text, user, uid);
                item->addComment(comment);
            }

            items << item;
        }

        addItemsToList(items);
    }
}

#include "moc_NotesModel.cpp"
