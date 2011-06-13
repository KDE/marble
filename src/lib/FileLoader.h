//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//
#ifndef MARBLE_FILELOADER_H
#define MARBLE_FILELOADER_H

#include "GeoDataTypes.h"

#include <QtCore/QThread>
#include <QtCore/QString>

namespace Marble
{
class GeoDataContainer;
class GeoDataDocument;

class FileLoader : public QThread
{
    Q_OBJECT
    public:
        FileLoader( QObject* parent, const QString& file, DocumentRole role );
        FileLoader( QObject* parent, const QString& contents, const QString& name, DocumentRole role );
        virtual ~FileLoader();

        void run();
        QString path() const;

    Q_SIGNALS:
        void loaderFinished( FileLoader* );
        void newGeoDataDocumentAdded( GeoDataDocument* );
    private:
        void importKml( const QString& filename );
        void importKmlFromData();
        void loadFile(const QString &filename );
        void saveFile(const QString& filename );
        void savePlacemarks(QDataStream &out, const GeoDataContainer *container);
        void loadPntFile( const QString &fileName );

        void setupStyle( GeoDataDocument *doc, GeoDataContainer *container );
        void createFilterProperties( GeoDataContainer *container );
        int cityPopIdx( qint64 population ) const;
        int spacePopIdx( qint64 population ) const;
        int areaPopIdx( qreal area ) const;

        QString m_filepath;
        QString m_contents;
        DocumentRole m_documentRole;
        GeoDataDocument *m_document;
};

} // namespace Marble

#endif
