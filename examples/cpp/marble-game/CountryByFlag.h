// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Abhinav Gangwar <abhgang@gmail.com>
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
    explicit CountryByFlag(MarbleWidget *marbleWidget);
    ~CountryByFlag() override;

public Q_SLOTS:
    void initiateGame();
    void postQuestion(QObject *);

Q_SIGNALS:
    void gameInitialized();

private Q_SLOTS:
    // void displayResult( bool );

private:
    CountryByFlagPrivate *const d;
};

} // namespace Marble

#endif // MARBLE_COUNTRY_BY_FLAG
