//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//
#ifndef MARBLE_FILELOADER_H
#define MARBLE_FILELOADER_H

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
        FileLoader( QObject* parent, const QString& file );
        FileLoader( QObject* parent, const QString& contents, const QString& name );
        virtual ~FileLoader();

        void run();
        QString path() const;

    Q_SIGNALS:
        void loaderFinished( FileLoader* );
        void newGeoDataDocumentAdded( GeoDataDocument* );
    private:
        void importKml( const QString& filename );
        void importKmlFromData();
        bool loadFile(const QString &filename );
        void saveFile(const QString& filename );
        void savePlacemarks(QDataStream &out, const GeoDataContainer *container);
        bool loadPntFile( const QString &fileName );

        QString m_filepath;
        QString m_contents;
        GeoDataDocument *m_document;
};

} // namespace Marble

#endif
