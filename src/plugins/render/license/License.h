//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2012 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
//

#ifndef MARBLELICENSE_H
#define MARBLELICENSE_H

#include "AbstractFloatItem.h"

class QLabel;

namespace Marble
{

class WidgetGraphicsItem;

/**
 * @short The class that displays copyright info
 *
 */

class License : public AbstractFloatItem
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.kde.marble.License")
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
    QVector<PluginAuthor> pluginAuthors() const override;
    QIcon icon () const;
    void initialize ();
    bool isInitialized () const;

private Q_SLOTS:
    void updateLicenseText();
    void toggleLicenseSize();
    void showAboutDialog();

protected:
    bool eventFilter(QObject *, QEvent *e);
    void contextMenuEvent( QWidget *w, QContextMenuEvent *e );

private:
    WidgetGraphicsItem* m_widgetItem;
    QLabel* m_label;
    bool m_showFullLicense;
    QMenu* m_contextMenu;
};

}

#endif
