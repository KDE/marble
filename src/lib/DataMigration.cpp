//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

// std
#include <limits>

// Qt
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QDirIterator>
#include <QtCore/QFileInfo>
#include <QtCore/QPointer>
#include <QtCore/QStack>
#include <QtGui/QDialog>
#include <QtGui/QProgressDialog>

// Marble
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "ui_DataMigrationWidget.h"

// Self
#include "DataMigration.h"

using namespace Marble;

DataMigration::DataMigration( QObject *parent )
    : QObject( parent )
{
}

DataMigration::~DataMigration()
{
}

void DataMigration::exec()
{
    QStringList oldLocalPaths = MarbleDirs::oldLocalPaths();

    if( oldLocalPaths.isEmpty() ) {
        return;
    }

    QString currentLocalPath = MarbleDirs::localPath();
    QDir currentLocalDir( currentLocalPath );
    if( currentLocalDir.entryList( QDir::AllEntries | QDir::NoDotAndDotDot ).size() != 0 ) {
        return;
    }

    foreach( const QString& oldLocalPath, oldLocalPaths ) {
        QDir oldLocalDir( oldLocalPath );

        if( oldLocalDir.entryList( QDir::AllEntries | QDir::NoDotAndDotDot ).size() == 0 ) {
            continue;
        }

        QPointer<QDialog> dialog = new QDialog();
        Ui::DataMigrationWidget dataMigrationWidget;

        dataMigrationWidget.setupUi( dialog );
        if( dialog->exec() == QDialog::Accepted ) {
            DataMigration::moveFiles( oldLocalPath, currentLocalPath );
        }
        delete dialog;

        return;
    }
}

void DataMigration::moveFiles( const QString& source, const QString& target )
{
    if( !QDir().rmdir( target ) ) {
        mDebug() << "Removing of the target directory failed";
    }

    // Trying to simply rename the directory. This is the fastest method, but it is not allways
    // possible. For example when the directories are on different file systems.
    // If the renaming of the directory is not successful, we have to copy and delete each
    // file separatetly.
    mDebug() << "Rename" << source << "to" << target;
    if( !QDir().rename( source, target ) ) {
        mDebug() << "Simple renaming of the data directory failed. Moving single files";

        QProgressDialog progressDialog;
        progressDialog.setWindowModality( Qt::WindowModal );
        progressDialog.setMinimum( 0 );
        progressDialog.setMaximum( std::numeric_limits<int>::max() );
        progressDialog.setAutoReset( false );
        progressDialog.setAutoClose( false );
        progressDialog.setWindowTitle( tr( "Marble data conversion" ) );
        progressDialog.setLabelText( tr( "Converting data ..." ) );

        QDir().mkpath( target );
        QString sourcePath = QDir( source ).canonicalPath();
        int sourcePathLength = sourcePath.length();

        // Running through all files recursively
        QStack<QString> dirs;
        dirs.push( sourcePath );

        QStack<int> progressSliceSizeStack;
        progressSliceSizeStack.push( progressDialog.maximum() );
        int progress = 0;

        while( !dirs.isEmpty() ) {
            if( progressDialog.wasCanceled() ) {
                return;
            }

            if( QDir( dirs.top() ).entryList( QDir::Dirs
                                              | QDir::Files
                                              | QDir::NoSymLinks
                                              | QDir::NoDotAndDotDot ).size() == 0 )
            {
                // Remove empty directory
                QDir().rmdir( dirs.pop() );
                progressSliceSizeStack.pop();
            }
            else {
                QString sourceDirPath = dirs.top();

                if( !sourceDirPath.startsWith( sourcePath ) ) {
                    dirs.pop();
                    continue;
                }

                QDir sourceDir( sourceDirPath );
                QStringList childDirs = sourceDir.entryList( QDir::Dirs
                                                             | QDir::NoSymLinks
                                                             | QDir::NoDotAndDotDot );

                // Add child directories to the stack
                foreach( const QString& childDir, childDirs ) {
                    dirs.push( sourceDirPath + '/' + childDir );
                }

                // Creating target dir
                QString targetDirPath = sourceDirPath;
                targetDirPath.remove( 0, sourcePathLength );
                targetDirPath.prepend( target );
                QDir().mkpath( targetDirPath );

                progressSliceSizeStack.push( progressSliceSizeStack.top() / ( childDirs.size() + 1 ) );
                // Copying contents
                QStringList files = sourceDir.entryList( QDir::Files
                                                         | QDir::NoSymLinks
                                                         | QDir::NoDotAndDotDot );
                foreach( const QString& file, files ) {
                    if( progressDialog.wasCanceled() ) {
                        return;
                    }

                    QString sourceFilePath = sourceDirPath;
                    sourceFilePath += '/';
                    sourceFilePath += file;

                    if( !sourceFilePath.startsWith( sourcePath ) ) {
                        continue;
                    }

                    QString targetFilePath = sourceFilePath;
                    targetFilePath.remove( 0, sourcePathLength );
                    targetFilePath.prepend( target );

                    QFile::copy( sourceFilePath, targetFilePath );
                    QFile::remove( sourceFilePath );
                }

                progress += progressSliceSizeStack.top();
                progressDialog.setValue( progress );
            }
        }
    }
}

#include "DataMigration.moc"
