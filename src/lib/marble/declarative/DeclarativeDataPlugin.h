//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Anton Chernov <chernov.anton.mail@gmail.com>
// Copyright 2012      "LOTES TM" LLC <lotes.sis@gmail.com>
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef DECLARATIVEDATAPLUGIN_H
#define DECLARATIVEDATAPLUGIN_H

#include "AbstractDataPlugin.h"

#include <QQmlComponent>
#include <QStringList>

class DeclarativeDataPluginPrivate;
namespace Marble { class MarbleModel; }

class DeclarativeDataPlugin: public Marble::AbstractDataPlugin
{
    Q_OBJECT
    Q_INTERFACES( Marble::RenderPluginInterface )

    Q_PROPERTY( QString planet READ planet WRITE setPlanet NOTIFY planetChanged )
    Q_PROPERTY( QString name READ name WRITE setName NOTIFY nameChanged )
    Q_PROPERTY( QString nameId READ nameId WRITE setNameId NOTIFY nameIdChanged )
    Q_PROPERTY( QString guiString READ guiString WRITE setGuiString NOTIFY guiStringChanged )
    Q_PROPERTY( QString version READ version WRITE setVersion NOTIFY versionChanged )
    Q_PROPERTY( QString copyrightYears READ copyrightYears WRITE setCopyrightYears NOTIFY copyrightYearsChanged )
    Q_PROPERTY( QString description READ description WRITE setDescription NOTIFY descriptionChanged )
    Q_PROPERTY( QStringList pluginAuthors READ authors WRITE setAuthors NOTIFY authorsChanged )
    Q_PROPERTY( QString aboutDataText READ aboutDataText WRITE setAboutDataText NOTIFY aboutDataTextChanged )
    Q_PROPERTY( QVariant model READ declarativeModel WRITE setDeclarativeModel NOTIFY declarativeModelChanged )
    Q_PROPERTY( QQmlComponent* delegate READ delegate WRITE setDelegate NOTIFY delegateChanged )

public:
    Marble::RenderPlugin* newInstance( const Marble::MarbleModel *marbleModel ) const override;

    explicit DeclarativeDataPlugin( const Marble::MarbleModel *marbleModel=0 );

    ~DeclarativeDataPlugin() override;

    QString planet() const;

    void setPlanet( const QString & planet );

    QString name() const override;

    void setName( const QString & name );

    QString guiString() const override;

    void setGuiString( const QString & guiString );

    QString nameId() const override;

    void setNameId( const QString & nameId );

    QString version() const override;

    void setVersion( const QString & version );

    QString copyrightYears() const override;

    void setCopyrightYears( const QString & copyrightYears );

    QString description() const override;

    void setDescription( const QString & description );

    QVector<Marble::PluginAuthor> pluginAuthors() const override;

    void setAuthors( const QStringList & pluginAuthors );

    QStringList authors() const;

    QString aboutDataText() const override;

    void setAboutDataText( const QString & aboutDataText );

    QIcon icon() const override;

    QQmlComponent *delegate();

    void setDelegate( QQmlComponent* delegate );

    QVariant declarativeModel();

    void setDeclarativeModel( const QVariant &model );

    void initialize() override;

    bool isInitialized() const override;

    Marble::RenderState renderState() const override;

Q_SIGNALS:
    /** Additional data for the currently visible map region is requested */
    void dataRequest( qreal north, qreal south, qreal east, qreal west );

    void planetChanged();

    void nameChanged();

    void nameIdChanged();

    void guiStringChanged();

    void versionChanged();

    void copyrightYearsChanged();

    void descriptionChanged();

    void authorsChanged();

    void aboutDataTextChanged();

    void declarativeModelChanged();

    void delegateChanged();

private:

    DeclarativeDataPluginPrivate *d;
};

#endif // DECLARATIVEDATAPLUGIN_H
