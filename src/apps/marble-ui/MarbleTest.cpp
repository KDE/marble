//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include "MarbleTest.h"

#include <QCoreApplication>
#include <QTime>
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

    QTime t;
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

    qDebug( "Timedemo finished in %ims", t.elapsed() );
    qDebug() <<  QString("= %1 fps").arg(200*1000/(qreal)(t.elapsed()));

}
