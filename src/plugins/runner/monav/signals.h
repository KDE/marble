/*
Copyright 2010  Christian Vetter veaac.fdirct@gmail.com

This file is part of MoNav.

MoNav is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

MoNav is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with MoNav.  If not, see <http://www.gnu.org/licenses/>.

Alternatively, this file may be used under the terms of the GNU Lesser
General Public License version 3 as published by the Free Software
Foundation.
*/

#ifndef SIGNALS_H
#define SIGNALS_H

#include <QtCore/QIODevice>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QDataStream>
#include <QtCore/QBuffer>
#include <QtNetwork/QLocalSocket>

struct RoutingDaemonCoordinate {
	double latitude;
	double longitude;

	friend QDataStream& operator<< ( QDataStream& out, const RoutingDaemonCoordinate& coordinate )
	{
		out << coordinate.latitude;
		out << coordinate.longitude;
		return out;
	}

	friend QDataStream& operator>> ( QDataStream& in, RoutingDaemonCoordinate& coordinate )
	{
		in >> coordinate.latitude;
		in >> coordinate.longitude;
		return in;
	}
};

class RoutingDaemonCommand {

public:

	double lookupRadius;
	QString dataDirectory;
	QVector< RoutingDaemonCoordinate > waypoints;

	void post( QIODevice* out )
	{
		QByteArray buffer;
		QDataStream stream( &buffer, QIODevice::WriteOnly );
		stream << lookupRadius;
		stream << dataDirectory;
		stream << waypoints;
		qint32 size = buffer.size();
		out->write( ( const char* ) &size, sizeof( qint32 ) );
		out->write( buffer.data(), size );
	}

	bool read( QLocalSocket* in )
	{
		qint32 size;
		while ( in->bytesAvailable() < ( int ) sizeof( qint32 ) ) {
			if ( in->state() != QLocalSocket::ConnectedState )
				return false;
			in->waitForReadyRead( 100 );
		}

		in->read( ( char* ) &size, sizeof( quint32 ) );

		while ( in->bytesAvailable() < size ) {
			if ( in->state() != QLocalSocket::ConnectedState )
				return false;
			in->waitForReadyRead( 100 );
		}

		QByteArray buffer= in->read( size );
		QDataStream stream( buffer );
		stream >> lookupRadius;
		stream >> dataDirectory;
		stream >> waypoints;

		return true;
	}

};

class RoutingDaemonResult {

public:

	enum ResultType {
		LoadFail = 1, RouteFail = 2, Success = 3
	} type;

	double seconds;
	QVector< RoutingDaemonCoordinate > path;

	void post( QIODevice* out )
	{
		QByteArray buffer;
		QDataStream stream( &buffer, QIODevice::WriteOnly );
		stream << qint32( type );
		stream << seconds;
		stream << path;
		qint32 size = buffer.size();
		out->write( ( const char* ) &size, sizeof( qint32 ) );
		out->write( buffer.data(), size );
	}

	bool read( QLocalSocket* in )
	{
		qint32 size;
		while ( in->bytesAvailable() < ( int ) sizeof( qint32 ) ) {
			if ( in->state() != QLocalSocket::ConnectedState )
				return false;
			in->waitForReadyRead( 100 );
		}

		in->read( ( char* ) &size, sizeof( quint32 ) );

		while ( in->bytesAvailable() < size ) {
			if ( in->state() != QLocalSocket::ConnectedState )
				return false;
			in->waitForReadyRead( 100 );
		}

		QByteArray buffer= in->read( size );
		QDataStream stream( buffer );
		qint32 temp;
		stream >> temp;
		type = ( ResultType ) temp;
		stream >> seconds;
		stream >> path;

		return true;
	}

};

#endif // SIGNALS_H
