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

#include <QtGui/QWidget>
#include "ui_MarblePluginSettingsWidget.h"

#include "marble_export.h"

/**
 * @short A public class that adds methods to the UI Plugins Settings Widget.
 *
 */

class QStandardItemModel;

namespace Marble
{

class MarblePluginSettingsWidgetPrivate;

class MARBLE_EXPORT MarblePluginSettingsWidget : public QWidget, private Ui::MarblePluginSettingsWidget
{
    Q_OBJECT

 public:
    MarblePluginSettingsWidget( QWidget *parent = 0 );
    ~MarblePluginSettingsWidget();
    void setModel( QStandardItemModel* );

    void setAboutIcon( const QIcon& icon );
    void setConfigIcon( const QIcon& icon );

 Q_SIGNALS:
    void pluginListViewClicked();
    void aboutPluginClicked( QString nameId );
    void configPluginClicked( QString nameId );

 private:
    MarblePluginSettingsWidgetPrivate *d;
};

}

#endif
