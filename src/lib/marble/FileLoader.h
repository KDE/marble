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

#include <QThread>

class QString;

namespace Marble
{
class FileLoaderPrivate;
class PluginManager;
class GeoDataStyle;

class FileLoader : public QThread
{
    Q_OBJECT
    public:
        FileLoader(QObject* parent, const PluginManager *pluginManager, bool recenter, const QString& file,
                   const QString& property, const GeoDataStyle::Ptr &style, DocumentRole role, int renderOrder );
        FileLoader( QObject* parent, const PluginManager *pluginManager,
                    const QString& contents, const QString& name, DocumentRole role );
        virtual ~FileLoader();

        void run();
        bool recenter() const;
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
