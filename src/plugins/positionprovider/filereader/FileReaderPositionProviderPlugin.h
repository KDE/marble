//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konrad Enzensberger <e.konrad@mpegcode.com>
// Copyright 2011      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_FILEREADERPOSITIONPROVIDERPLUGIN_H
#define MARBLE_FILEREADERPOSITIONPROVIDERPLUGIN_H

#include "PositionProviderPlugin.h"

class QTimer;

namespace Marble
{

class FileReaderPositionProviderPluginPrivate;

class FileReaderPositionProviderPlugin: public PositionProviderPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::PositionProviderPluginInterface )

public:
    FileReaderPositionProviderPlugin();
    virtual ~FileReaderPositionProviderPlugin();

    // Implementing PluginInterface
    virtual QString name() const;
    virtual QString nameId() const;
    virtual QString guiString() const;
    virtual QString description() const;
    virtual QIcon icon() const;
    virtual void initialize();
    virtual bool isInitialized() const;
    virtual qreal speed() const;
    virtual qreal direction() const;

    // Implementing PositionProviderPlugin
    virtual PositionProviderPlugin * newInstance() const;

    // Implementing PositionProviderPluginInterface
    virtual PositionProviderStatus status() const;
    virtual GeoDataCoordinates position() const;
    virtual GeoDataAccuracy accuracy() const;

private Q_SLOTS:
    /** Regular (each second) position and status update */
    void update();

private:
    FileReaderPositionProviderPluginPrivate* const d;
};

}

#endif // MARBLE_FILEREADERPOSITIONPROVIDERPLUGIN_H
