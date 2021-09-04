// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef ABSTRACTWEATHERSERVICE_H
#define ABSTRACTWEATHERSERVICE_H

#include <QObject>
#include <QStringList>

class QUrl;

namespace Marble
{

class AbstractDataPluginItem;
class GeoDataLatLonAltBox;
class MarbleModel;
class MarbleWidget;

class AbstractWeatherService : public QObject
{
    Q_OBJECT
    
 public:
    explicit AbstractWeatherService( const MarbleModel *model, QObject *parent );
    ~AbstractWeatherService() override;
    void setMarbleWidget( MarbleWidget* widget );
    
 public Q_SLOTS:
    virtual void setFavoriteItems( const QStringList& favorite );
    QStringList favoriteItems() const;

    virtual void getAdditionalItems( const GeoDataLatLonAltBox& box,
                                     qint32 number = 10 ) = 0;
    virtual void getItem( const QString &id ) = 0;
    virtual void parseFile( const QByteArray& file );
    
 Q_SIGNALS:
    void requestedDownload( const QUrl& url, const QString& type, AbstractDataPluginItem *item );
    void createdItems( const QList<AbstractDataPluginItem*>& items );
    void downloadDescriptionFileRequested( const QUrl& );

protected:
    const MarbleModel* marbleModel() const;
    MarbleWidget* marbleWidget();

private:
    const MarbleModel *const m_marbleModel;
    QStringList m_favoriteItems;
    MarbleWidget* m_marbleWidget;
};

} // namespace Marble

#endif // ABSTRACTWEATHERSERVICE_H
