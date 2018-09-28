//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Bastian Holst <bastianholst@gmx.de>
//

// Self
#include "DataMigration.h"

// Marble
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "ui_DataMigrationWidget.h"

// Qt
#include <QDebug>
#include <QDir>
#include <QDirIterator>
#include <QPointer>
#include <QStack>
#include <QDialog>
#include <QProgressDialog>

// std
#include <limits>

namespace Marble
{

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

    for( const QString& oldLocalPath: oldLocalPaths ) {
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

    // Trying to simply rename the directory. This is the fastest method, but it is not always
    // possible. For example when the directories are on different file systems.
    // If the renaming of the directory is not successful, we have to copy and delete each
    // file separately.
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

            QString sourceDirPath = dirs.top();
            mDebug() << "DataMigration: Current source dir path ="
                     << sourceDirPath;
            mDebug() << "SliceSize =" << progressSliceSizeStack.top();

            if( !sourceDirPath.startsWith( sourcePath ) ) {
                dirs.pop();
                progress += progressSliceSizeStack.pop();
                progressDialog.setValue( progress );
                continue;
            }

            QDir sourceDir( sourceDirPath );
            // Creating child file/dir lists.
            QStringList files = sourceDir.entryList( QDir::Files
                                                     | QDir::NoSymLinks
                                                     | QDir::NoDotAndDotDot );
            QStringList childDirs = sourceDir.entryList( QDir::Dirs
                                                         | QDir::NoSymLinks
                                                         | QDir::NoDotAndDotDot );
            int childSliceSize = 0;
            if( !childDirs.isEmpty() ) {
                childSliceSize = progressSliceSizeStack.pop() / childDirs.size();
                progressSliceSizeStack.push( 0 );
            }

            if( files.isEmpty() && childDirs.isEmpty() )
            {
                // Remove empty directory
                mDebug() << "DataMigration:" << dirs.top()
                         << "finished";
                QDir().rmdir( dirs.pop() );
                progress += progressSliceSizeStack.pop();
                progressDialog.setValue( progress );
            }
            else {
                // Add child directories to the stack
                for( const QString& childDir: childDirs ) {
                    dirs.push(sourceDirPath + QLatin1Char('/') + childDir);
                    progressSliceSizeStack.push( childSliceSize );
                }

                // Creating target dir
                QString targetDirPath = sourceDirPath;
                targetDirPath.remove( 0, sourcePathLength );
                targetDirPath.prepend( target );
                QDir().mkpath( targetDirPath );

                // Copying contents
                for( const QString& file: files ) {
                    if( progressDialog.wasCanceled() ) {
                        return;
                    }

                    const QString sourceFilePath = sourceDirPath + QLatin1Char('/') + file;

                    if( !sourceFilePath.startsWith( sourcePath ) ) {
                        continue;
                    }

                    QString targetFilePath = sourceFilePath;
                    targetFilePath.remove( 0, sourcePathLength );
                    targetFilePath.prepend( target );

                    QFile::copy( sourceFilePath, targetFilePath );
                    QFile::remove( sourceFilePath );
                }
            }
        }
    }
}

}

#include "moc_DataMigration.cpp"
