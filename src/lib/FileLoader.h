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

#include "GeoDataDocument.h"
#include "GeoDataStyle.h"

#include <QtCore/QThread>
#include <QtCore/QString>

namespace Marble
{
class GeoDataContainer;
class FileLoaderPrivate;
class MarbleModel;

class FileLoader : public QThread
{
    Q_OBJECT
    public:
        FileLoader( QObject* parent, MarbleModel *model,
                    const QString& file, const QString& property, GeoDataStyle* style, DocumentRole role );
        FileLoader( QObject* parent, MarbleModel *model,
                    const QString& contents, const QString& name, DocumentRole role );
        virtual ~FileLoader();

        void run();
        QString path() const;
        GeoDataDocument *document();
        QString error() const;

    Q_SIGNALS:
        void loaderFinished( FileLoader* );
        void newGeoDataDocumentAdded( GeoDataDocument* );

private:
        Q_PRIVATE_SLOT ( d, void documentParsed( GeoDataDocument *, QString) )

        friend class FileLoaderPrivate;

        FileLoaderPrivate *d;

};

} // namespace Marble

#endif
