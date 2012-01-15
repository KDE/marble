//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <earthwings@gentoo.org>
//

#include "SpeakersModel.h"

#include "MarbleDirs.h"

namespace Marble
{

SpeakersModel::SpeakersModel( QObject *parent ) :
    QAbstractListModel( parent )
{
    QHash<int,QByteArray> roles = roleNames();
    roles[Qt::UserRole+1] = "path";
    setRoleNames( roles );

    fillModel();
}

SpeakersModel::~SpeakersModel()
{
    // nothing to do
}

int SpeakersModel::rowCount ( const QModelIndex &parent ) const
{
    if ( !parent.isValid() ) {
        return m_speakers.size();
    }

    return 0;
}

QVariant SpeakersModel::data ( const QModelIndex &index, int role ) const
{
    if ( index.isValid() && index.row() >= 0 && index.row() < m_speakers.size() ) {
        switch ( role ) {
        case Qt::DisplayRole: return m_speakers.at( index.row() ).fileName();
        case Qt::UserRole+1: return m_speakers.at( index.row() ).absoluteFilePath();
        }
    }

    return QVariant();
}

int SpeakersModel::indexOf( const QString &name )
{
    for( int i=0; i<m_speakers.size(); ++i ) {
        if ( m_speakers[i].absoluteFilePath() == name ) {
            return i;
        }
    }
    return -1;
}

void SpeakersModel::fillModel()
{
    m_speakers.clear();

    QStringList const baseDirs = QStringList() << MarbleDirs::systemPath() << MarbleDirs::localPath();
    foreach ( const QString &baseDir, baseDirs ) {
        QString base = baseDir + "/audio/speakers/";

        QDir::Filters filter = QDir::Readable | QDir::Dirs | QDir::NoDotAndDotDot;
        m_speakers << QDir( base ).entryInfoList( filter, QDir::Name );
    }

    reset();
}

}

#include "SpeakersModel.moc"
