//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>
//

#ifndef COMPASS_FLOAT_ITEM_H
#define COMPASS_FLOAT_ITEM_H

#include <QPixmap>

#include "AbstractFloatItem.h"
#include "DialogConfigurationInterface.h"

class QSvgRenderer;

namespace Ui {
    class CompassConfigWidget;
}

namespace Marble
{

/**
 * @short The class that creates a compass
 *
 */

class CompassFloatItem  : public AbstractFloatItem, public DialogConfigurationInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.CompassFloatItem")
    Q_INTERFACES( Marble::RenderPluginInterface )
    Q_INTERFACES( Marble::DialogConfigurationInterface )
    MARBLE_PLUGIN( CompassFloatItem )
 public:
    CompassFloatItem();
    explicit CompassFloatItem( const MarbleModel *marbleModel );
    ~CompassFloatItem ();

    QStringList backendTypes() const;

    QString name() const;

    QString guiString() const;

    QString nameId() const;

    QString version() const;

    QString description() const;

    QString copyrightYears() const;

    QVector<PluginAuthor> pluginAuthors() const override;

    QIcon icon () const;
    
    void initialize ();

    bool isInitialized () const;

    QPainterPath backgroundShape() const;

    void setProjection( const ViewportParams *viewport );

    void paintContent( QPainter *painter );

    QDialog *configDialog();

    QHash<QString,QVariant> settings() const;

    void setSettings( const QHash<QString,QVariant> &settings );

private Q_SLOTS:
   void readSettings();

   void writeSettings();

 private:
    Q_DISABLE_COPY( CompassFloatItem )

    bool           m_isInitialized;

    QSvgRenderer  *m_svgobj;
    QPixmap        m_compass;

    /// allowed values: -1, 0, 1; default here: 0. FIXME: Declare enum
    int            m_polarity;

    int m_themeIndex;
    QDialog * m_configDialog;
    Ui::CompassConfigWidget * m_uiConfigWidget;
};
}

#endif
