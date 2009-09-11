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

#include "GeoDataPlacemark.h"

namespace Marble
{
class GeoDataDocument;

class PlacemarkLoader : public QThread
{
    Q_OBJECT
    public:
        PlacemarkLoader( QObject* parent, const QString& file );
        PlacemarkLoader( QObject* parent, const QString& contents, const QString& name );
        virtual ~PlacemarkLoader();

        void run();
        QString path() const;
        
    Q_SIGNALS:
        void placemarksLoaded( PlacemarkLoader*, QVector<Marble::GeoDataPlacemark> * );
        void placemarkLoaderFailed( PlacemarkLoader* );
        void newGeoDataDocumentAdded( GeoDataDocument* );
    private:
        bool loadFile( const QString& filename );
        void saveFile( const QString& filename );
        void importKml( const QString& filename );
        void importKmlFromData();

        QString m_filepath;
        QString m_contents;
        GeoDataDocument *m_document;
        QVector<Marble::GeoDataPlacemark> *m_container;
};

} // namespace Marble
#endif // PLACEMARKLOADER_H
