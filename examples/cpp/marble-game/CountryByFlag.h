//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014 Abhinav Gangwar <abhgang@gmail.com>
//

#ifndef MARBLE_COUNTRY_BY_FLAG
#define MARBLE_COUNTRY_BY_FLAG

// Qt
#include <QObject>

namespace Marble
{
class CountryByFlagPrivate;
class MarbleWidget;

class CountryByFlag : public QObject
{
    Q_OBJECT;
public:
    explicit CountryByFlag( MarbleWidget *marbleWidget );
    ~CountryByFlag();

public Q_SLOTS:
    void initiateGame();
    void postQuestion( QObject* );

Q_SIGNALS:
    void gameInitialized();
    
private Q_SLOTS:
    //void displayResult( bool );

private:
    CountryByFlagPrivate * const d;
};

}   // namespace Marble

#endif   // MARBLE_COUNTRY_BY_FLAG
