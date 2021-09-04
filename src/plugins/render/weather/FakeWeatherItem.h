//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef FAKEWEATHERITEM_H
#define FAKEWEATHERITEM_H

#include "WeatherItem.h"

class QString;

namespace Marble
{

class FakeWeatherItem : public WeatherItem
{
 public:
    explicit FakeWeatherItem( QObject *parent = nullptr );
    ~FakeWeatherItem() override;
    
    QString service() const override;
    void addDownloadedFile( const QString& url, const QString& type ) override;
};

} // namespace Marble

#endif // FAKEWEATHERITEM_H
