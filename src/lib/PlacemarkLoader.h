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
class PlacemarkContainer;
class GeoDataDocument;

class PlacemarkLoader : public QThread {
    Q_OBJECT
    public:
        PlacemarkLoader( QObject* parent, const QString& file, bool finalize = true );
        PlacemarkLoader( QObject* parent, const QString& contents, const QString& name, bool finalize = true );

        bool finalize();
        void run();
        
    Q_SIGNALS:
        void placemarksLoaded( PlacemarkLoader*, PlacemarkContainer * );
        void placemarkLoaderFailed( PlacemarkLoader* );
        void newGeoDataDocumentAdded( GeoDataDocument* );
    private:
        bool loadFile( const QString& filename, PlacemarkContainer* placemarkContainer );
        void saveFile( const QString& filename, PlacemarkContainer* placemarkContainer );
        void importKml( const QString& filename, PlacemarkContainer* placemarkContainer );
        void importKmlFromData( PlacemarkContainer* placemarkContainer );

        QString m_filepath;
        QString m_contents;
        bool m_finalize;
};

} // namespace Marble
#endif // PLACEMARKLOADER_H
