//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#include "SpeakersModel.h"

#include "MarbleDirs.h"
#include "NewstuffModel.h"

namespace Marble
{

class SpeakersModelItem
{
public:
    QFileInfo m_file;

    int m_newstuffIndex;

    SpeakersModelItem();

    static bool lessThan( const SpeakersModelItem& one, const SpeakersModelItem& two );
};

class SpeakersModelPrivate
{
public:
    SpeakersModel* m_parent;

    QList<SpeakersModelItem> m_speakers;

    NewstuffModel m_newstuffModel;

    explicit SpeakersModelPrivate( SpeakersModel* parent );

    void fillModel();

    void handleInstallationProgress( int row, qreal progress );

    void handleInstallation( int );

    QHash<int, QByteArray> m_roleNames;
};

SpeakersModelItem::SpeakersModelItem() : m_newstuffIndex( -1 )
{
    // nothing to do
}

bool SpeakersModelItem::lessThan( const SpeakersModelItem& one, const SpeakersModelItem& two )
{
    return one.m_file.fileName() < two.m_file.fileName();
}

SpeakersModelPrivate::SpeakersModelPrivate( SpeakersModel* parent ) :
    m_parent( parent )
{
    m_newstuffModel.setTargetDirectory(MarbleDirs::localPath() + QLatin1String("/audio/speakers"));
    m_newstuffModel.setProvider( "http://edu.kde.org/marble/newstuff/speakers.xml" );
    QObject::connect( &m_newstuffModel, SIGNAL(modelReset()), m_parent, SLOT(fillModel()) );
    QObject::connect( &m_newstuffModel, SIGNAL(installationProgressed(int,qreal)),
                      m_parent, SLOT(handleInstallationProgress(int,qreal)) );
    QObject::connect( &m_newstuffModel, SIGNAL(installationFinished(int)), m_parent, SLOT(handleInstallation(int)) );
}

void SpeakersModelPrivate::fillModel()
{
    m_speakers.clear();

    QStringList const baseDirs = QStringList() << MarbleDirs::systemPath() << MarbleDirs::localPath();
    for ( const QString &baseDir: baseDirs ) {
        const QString base = baseDir + QLatin1String("/audio/speakers/");

        QDir::Filters filter = QDir::Readable | QDir::Dirs | QDir::NoDotAndDotDot;
        QFileInfoList subdirs = QDir( base ).entryInfoList( filter, QDir::Name );
        for( const QFileInfo &file: subdirs ) {
            SpeakersModelItem item;
            item.m_file = file;
            m_speakers << item;
        }
    }

    for ( int i=0; i<m_newstuffModel.rowCount(); ++i ) {
        QModelIndex const index = m_newstuffModel.index( i );
        QString const name = m_newstuffModel.data( index ).toString();
        bool exists = false;
        for ( int j=0; j<m_speakers.size() && !exists; ++j ) {
            SpeakersModelItem &item = m_speakers[j];
            if ( item.m_file.fileName() == name ) {
                item.m_newstuffIndex = i;
                exists = true;
            }
        }

        if ( !exists ) {
            SpeakersModelItem item;
            QString const path = "%1/audio/speakers/%2";
            item.m_file = QFileInfo( path.arg( MarbleDirs::localPath(), name ) );
            item.m_newstuffIndex = i;
            m_speakers << item;
        }
    }

    std::sort(m_speakers.begin(), m_speakers.end(), SpeakersModelItem::lessThan);
    m_parent->beginResetModel();
    m_parent->endResetModel();
    emit m_parent->countChanged();
}

void SpeakersModelPrivate::handleInstallation( int row )
{
    for ( int j=0; j<m_speakers.size(); ++j ) {
        if ( m_speakers[j].m_newstuffIndex == row ) {
            m_speakers[j].m_file.refresh();
            QModelIndex const affected = m_parent->index( j );
            emit m_parent->dataChanged( affected, affected );
            emit m_parent->installationFinished( j );
        }
    }
}

void SpeakersModelPrivate::handleInstallationProgress( int row, qreal progress )
{
    for ( int j=0; j<m_speakers.size(); ++j ) {
        if ( m_speakers[j].m_newstuffIndex == row ) {
            emit m_parent->installationProgressed( j, progress );
        }
    }
}

SpeakersModel::SpeakersModel( QObject *parent ) :
    QAbstractListModel( parent ), d( new SpeakersModelPrivate( this ) )
{
    QHash<int,QByteArray> roles;
    roles[Path] = "path";
    roles[Name] = "name";
    roles[IsLocal] = "isLocal";
    roles[IsRemote] = "isRemote";
    d->m_roleNames = roles;

    d->fillModel();
}

SpeakersModel::~SpeakersModel()
{
    delete d;
}

int SpeakersModel::rowCount ( const QModelIndex &parent ) const
{
    if ( !parent.isValid() ) {
        return d->m_speakers.size();
    }

    return 0;
}

QVariant SpeakersModel::data ( const QModelIndex &index, int role ) const
{
    if ( index.isValid() && index.row() >= 0 && index.row() < d->m_speakers.size() ) {
        switch ( role ) {
        case Qt::DisplayRole: return d->m_speakers.at( index.row() ).m_file.fileName();
        case Path: return d->m_speakers.at( index.row() ).m_file.absoluteFilePath();
        case Name: return d->m_speakers.at( index.row() ).m_file.fileName();
        case IsLocal: return d->m_speakers.at( index.row() ).m_file.exists();
        case IsRemote: return d->m_speakers.at( index.row() ).m_newstuffIndex >= 0;
        }
    }

    return QVariant();
}

QHash<int, QByteArray> SpeakersModel::roleNames() const
{
    return d->m_roleNames;
}

int SpeakersModel::indexOf( const QString &name )
{
    for( int i=0; i<d->m_speakers.size(); ++i ) {
        if ( d->m_speakers[i].m_file.absoluteFilePath() == name || d->m_speakers[i].m_file.fileName() == name ) {
            return i;
        }
    }
    return -1;
}

QString SpeakersModel::path( int index )
{
    if ( index >= 0 && index <= d->m_speakers.size() ) {
        return d->m_speakers[index].m_file.absoluteFilePath();
    }
    return QString();
}

void SpeakersModel::install( int index )
{
    if ( index >= 0 && index < d->m_speakers.size() ) {
        if ( d->m_speakers[index].m_newstuffIndex >= 0 ) {
            d->m_newstuffModel.install( d->m_speakers[index].m_newstuffIndex );
        }
    }
}

bool SpeakersModel::isLocal( int idx ) const
{
    return data( index( idx ), IsLocal ).toBool();
}

bool SpeakersModel::isRemote( int idx ) const
{
    return data( index( idx ), IsRemote ).toBool();
}

int SpeakersModel::count() const
{
    return rowCount();
}

}

#include "moc_SpeakersModel.cpp"
