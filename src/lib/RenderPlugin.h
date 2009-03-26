//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
// Copyright 2008 Inge Wallin  <inge@lysator.liu.se>"
//


#ifndef MARBLE_RENDER_PLUGIN_H
#define MARBLE_RENDER_PLUGIN_H

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/Qt>

#include "RenderPluginInterface.h"
#include "marble_export.h"


class QAction;
class QStandardItem;

namespace Marble
{

class RenderPluginPrivate;
class MarbleDataFacade;

/**
 * @short The abstract class that creates an "empty" float item.
 *
 */

class MARBLE_EXPORT RenderPlugin : public QObject, public RenderPluginInterface
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )

 public:
    RenderPlugin();
    virtual ~RenderPlugin();

    MarbleDataFacade* dataFacade() const;
    void  setDataFacade( MarbleDataFacade* );

    QAction       *action() const;
    QStandardItem *item()   const;

    void applyItemState();
    void retrieveItemState();

    bool    enabled() const;
    bool    visible() const;

 public Q_SLOTS:
    void    setEnabled( bool enabled );
    void    setVisible( bool visible );

 Q_SIGNALS:
    void    valueChanged( QString nameId, bool visible );

 private:
    Q_DISABLE_COPY( RenderPlugin )
    RenderPluginPrivate * const d;
};

#define MARBLE_PLUGIN(T) public:\
    virtual RenderPlugin* pluginInstance() { return new T(); };
}

#endif // MARBLE_RENDER_PLUGIN_H
