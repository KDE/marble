//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_MARBLEPLUGINSETTINGSWIDGET_H
#define MARBLE_MARBLEPLUGINSETTINGSWIDGET_H


/** @file
 * This file contains the multiple inheritance ui-wrapper for the
 * MarblePluginSettingsWidget ui file.
 *
 * @author Torsten Rahn  <torsten@kde.org>
 */

#include <QWidget>

#include "marble_export.h"

class QModelIndex;

/**
 * @short A public class that adds methods to the UI Plugins Settings Widget.
 *
 */
namespace Marble
{

class MarblePluginSettingsWidgetPrivate;
class RenderPluginModel;

class MARBLE_EXPORT MarblePluginSettingsWidget : public QWidget
{
    Q_OBJECT

 public:
    explicit MarblePluginSettingsWidget( QWidget *parent = 0 );
    ~MarblePluginSettingsWidget();

    void setModel( RenderPluginModel* pluginModel );

    void setAboutIcon( const QIcon& icon );
    void setConfigIcon( const QIcon& icon );

 Q_SIGNALS:
    void pluginListViewClicked();

 private:
    Q_PRIVATE_SLOT( d, void showPluginAboutDialog( const QModelIndex & ) )
    Q_PRIVATE_SLOT( d, void showPluginConfigDialog( const QModelIndex & ) )

 private:
    MarblePluginSettingsWidgetPrivate *d;
};

}

#endif
