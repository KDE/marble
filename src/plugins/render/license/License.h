//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <earthwings@gentoo.org>
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef MARBLELICENSE_H
#define MARBLELICENSE_H

#include <QtCore/QObject>

#include "AbstractFloatItem.h"

class QLabel;

namespace Marble
{

class GeoDataCoordinates;
class WidgetGraphicsItem;
class MarbleLocale;

/**
 * @short The class that displays copyright info
 *
 */

class License : public AbstractFloatItem
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )
    MARBLE_PLUGIN( License )
public:
    explicit License( const MarbleModel *marbleModel=0 );
    ~License();

    QStringList backendTypes() const;
    QString name() const;
    QString guiString() const;
    QString nameId() const;
    QString version() const;
    QString description() const;
    QString copyrightYears() const;
    QList<PluginAuthor> pluginAuthors() const;
    QIcon icon () const;
    void initialize ();
    bool isInitialized () const;

public slots:
    void updateLicenseText();

protected:
    bool eventFilter(QObject *, QEvent *e);

private:
    WidgetGraphicsItem* m_widgetItem;
    QLabel* m_label;
};

}

#endif
