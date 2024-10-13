// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>
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
    FileLoader(QObject *parent,
               const PluginManager *pluginManager,
               bool recenter,
               const QString &file,
               const QString &property,
               const GeoDataStyle::Ptr &style,
               DocumentRole role,
               int renderOrder);
    FileLoader(QObject *parent, const PluginManager *pluginManager, const QString &contents, const QString &name, DocumentRole role);
    ~FileLoader() override;

    void run() override;
    bool recenter() const;
    QString path() const;
    GeoDataDocument *document();
    QString error() const;

Q_SIGNALS:
    void loaderFinished(FileLoader *);
    void newGeoDataDocumentAdded(GeoDataDocument *);

private:
    Q_PRIVATE_SLOT(d, void documentParsed(GeoDataDocument *, QString))

    friend class FileLoaderPrivate;

    FileLoaderPrivate *const d;
};

} // namespace Marble

#endif
