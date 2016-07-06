//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#ifndef MARBLE_FILEMANAGER_H
#define MARBLE_FILEMANAGER_H

#include "GeoDataDocument.h"

#include <QObject>

class QString;

namespace Marble
{

class FileManagerPrivate;
class FileLoader;
class GeoDataLatLonBox;
class GeoDataTreeModel;
class PluginManager;

/**
 * This class is responsible for loading the
 * different files into Geodata model.
 *
 * The loaded data are accessible via
 * various models in MarbleModel.
 */
class FileManager : public QObject
{
    Q_OBJECT

 public:
    /**
     * Creates a new file manager.
     *
     * @param parent The parent object.
     */
    explicit FileManager( GeoDataTreeModel *treeModel, const PluginManager *pluginManager, QObject *parent = 0 );

    /**
     * Destroys the file manager.
     */
    ~FileManager();

    /**
     * Loads a new file into the manager.
     */
    void addFile(const QString &fileName, const QString &property, const GeoDataStyle::Ptr &style, DocumentRole role, int renderOrder = 0, bool recenter = false );


    /**
    * removes an existing file from the manager
    */
    void removeFile( const QString &fileName );

    /**
    * add Data containing KML code as string
    */
    void addData( const QString &name, const QString &data, DocumentRole role );

    void saveFile( const QString &fileName, const GeoDataDocument *document );
    void closeFile( const GeoDataDocument *document );

    int size() const;
    GeoDataDocument *at( const QString &key );

    /** Returns the number of files being opened at the moment */
    int pendingFiles() const;

 Q_SIGNALS:
    void fileAdded( const QString &key );
    void fileRemoved( const QString &key );
    void centeredDocument( const GeoDataLatLonBox& );

 private:

    Q_PRIVATE_SLOT( d, void cleanupLoader( FileLoader *loader ) )

    Q_DISABLE_COPY( FileManager )

    friend class FileManagerPrivate;
    FileManagerPrivate *const d;
};

}

#endif
