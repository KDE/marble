//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


#ifndef MARBLE_ABSTRACT_LAYER_H
#define MARBLE_ABSTRACT_LAYER_H

#include <QtCore/QObject>
#include <QtCore/Qt>

#include "MarbleLayerInterface.h"
#include "marble_export.h"

class QAction;

class MarbleAbstractLayerPrivate;


/**
 * @short The abstract class that creates an "empty" float item.
 *
 */

class MARBLE_EXPORT MarbleAbstractLayer : public QObject, public MarbleLayerInterface
{
    Q_OBJECT
    Q_INTERFACES( MarbleLayerInterface )

 public:
    explicit MarbleAbstractLayer();
    virtual ~MarbleAbstractLayer();

    QAction* action() const;

    bool    visible() const;

 public Q_SLOTS:
    void    setVisible( bool visible );

 Q_SIGNALS:
    void    valueChanged( QString nameId, bool visible );

 private:
    Q_DISABLE_COPY( MarbleAbstractLayer )
    MarbleAbstractLayerPrivate  * const d;
};

#endif // MARBLE_ABSTRACT_LAYER_H
