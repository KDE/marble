//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//
#ifndef PLACEMARKLOADER_H
#define PLACEMARKLOADER_H

#include <QtCore/QString>
#include <QtCore/QThread>

namespace Marble
{
class MarblePlacemarkModel;
class PlaceMarkContainer;
class GeoDataDocument;

class PlaceMarkLoader : public QThread {
    Q_OBJECT
    public:
        PlaceMarkLoader( QObject* parent, const QString& file );
        PlaceMarkLoader( QObject* parent, const QString& contents, const QString& name );

        void run();
    Q_SIGNALS:
        void placeMarksLoaded( PlaceMarkLoader*, PlaceMarkContainer * );
        void placeMarkLoaderFailed( PlaceMarkLoader* );
        void newGeoDataDocumentAdded( GeoDataDocument* );
    private:
        bool loadFile( const QString& filename, PlaceMarkContainer* placeMarkContainer );
        void saveFile( const QString& filename, PlaceMarkContainer* placeMarkContainer );
        void importKml( const QString& filename, PlaceMarkContainer* placeMarkContainer );
        void importKmlFromData( PlaceMarkContainer* placeMarkContainer );

        QString m_filepath;
        QString m_contents;
};

} // namespace Marble
#endif // PLACEMARKLOADER_H
