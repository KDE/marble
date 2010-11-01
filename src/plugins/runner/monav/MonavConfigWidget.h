//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Dennis Nienhüser <earthwings@gentoo.org>
//

#ifndef MARBLE_MONAVCONFIGWIDGET_H
#define MARBLE_MONAVCONFIGWIDGET_H

#include "ui_MonavConfigWidget.h"
#include "RunnerPlugin.h"

#include <QtGui/QWidget>

class QNetworkReply;

namespace Marble
{

class MonavPlugin;
class MonavConfigWidgetPrivate;

class MonavConfigWidget : public RunnerPlugin::ConfigWidget, private Ui::MonavConfigWidget
{
    Q_OBJECT

public:
    explicit MonavConfigWidget( MonavPlugin* plugin );

    ~MonavConfigWidget();

    virtual void loadSettings( const QHash<QString, QVariant> &settings );

    virtual QHash<QString, QVariant> settings() const;

protected:
    virtual void showEvent ( QShowEvent * event );

private Q_SLOTS:
    void retrieveData( QNetworkReply *reply );

    void updateComboBoxes();

    void updateStates();

    void updateRegions();

    void downloadMap();

    void updateProgressBar( qint64 bytesReceived, qint64 bytesTotal );

    void mapInstalled( int exitStatus );

    void updateTransportTypeFilter( const QString &filter );

    void removeMap( int index );

    void upgradeMap( int index );

    void cancelOperation();

private:
    friend class MonavConfigWidgetPrivate;
    MonavConfigWidgetPrivate* const d;
};

}

#endif // MARBLE_MONAVCONFIGWIDGET_H
