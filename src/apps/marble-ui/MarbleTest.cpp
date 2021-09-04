// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
//

#include "MarbleTest.h"

#include <QCoreApplication>
#include <QElapsedTimer>
#include <QMessageBox>

#include <MarbleGlobal.h>
#include <MarbleDebug.h>
#include <MarbleWidget.h>
#include <MarbleModel.h>

using namespace Marble;

MarbleTest::MarbleTest( MarbleWidget* marbleWidget )
    : m_marbleWidget(marbleWidget)
{
}

void MarbleTest::timeDemo()
{
    QMessageBox::information(m_marbleWidget, QString( "Marble Speed Test" ), QString( "Press Ok to start test" ) );

    m_marbleWidget->setViewContext( Marble::Animation );

        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveRight( Instant );
            QCoreApplication::flush();
            m_marbleWidget->repaint();
        }
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveLeft( Instant );
            QCoreApplication::flush();
            m_marbleWidget->repaint();
        }

    qDebug() << "Starting Performance Test";

    QElapsedTimer t;
    t.start();

    for ( int j = 0; j < 10; ++j ) {
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveRight( Instant );
            QCoreApplication::flush();
            m_marbleWidget->repaint();
        }
        for ( int k = 0; k < 10; ++k ) {
            m_marbleWidget->moveLeft( Instant );
            QCoreApplication::flush();
            m_marbleWidget->repaint();
        }
    }

    qDebug( "Timedemo finished in %ims", static_cast<int>(t.elapsed()));
    qDebug() <<  QString("= %1 fps").arg(200*1000/(qreal)(t.elapsed()));

}
