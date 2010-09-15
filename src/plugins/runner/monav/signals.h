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

Alternatively, this file may be used under the terms of the GNU
Library General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option)
any later version.
*/

#ifndef SIGNALS_H
#define SIGNALS_H

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QDataStream>
#include <QtCore/QStringList>
#include <QtNetwork/QLocalSocket>

struct RoutingDaemonNode {
	double latitude;
	double longitude;

	friend QDataStream& operator<< ( QDataStream& out, const RoutingDaemonNode& node )
	{
		out << node.latitude;
		out << node.longitude;
		return out;
	}

	friend QDataStream& operator>> ( QDataStream& in, RoutingDaemonNode& node )
	{
		in >> node.latitude;
		in >> node.longitude;
		return in;
	}
};

struct RoutingDaemonEdge {
	unsigned length;
	unsigned name;
	unsigned type;

	friend QDataStream& operator<< ( QDataStream& out, const RoutingDaemonEdge& edge )
	{
		out << edge.length;
		out << edge.name;
		out << edge.type;
		return out;
	}

	friend QDataStream& operator>> ( QDataStream& in, RoutingDaemonEdge& edge )
	{
		in >> edge.length;
		in >> edge.name;
		in >> edge.type;
		return in;
	}
};

class RoutingDaemonCommand {

public:

	RoutingDaemonCommand()
	{
		lookupRadius = 10000; // 10km should suffice for most applications
		lookupStrings = false;
	}

	double lookupRadius;
	bool lookupStrings;
	QString dataDirectory;
	QVector< RoutingDaemonNode > waypoints;

	void post( QIODevice* out )
	{
		QByteArray buffer;
		QDataStream stream( &buffer, QIODevice::WriteOnly );
		stream << lookupRadius;
		stream << lookupStrings;
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
		stream >> lookupStrings;
		stream >> dataDirectory;
		stream >> waypoints;

		return true;
	}

};

class RoutingDaemonResult {

public:

	enum ResultType {
		LoadFailed = 1, RouteFailed = 2, NameLookupFailed = 3, TypeLookupFailed = 4, Success = 5
	} type;

	double seconds;
	QVector< RoutingDaemonNode > pathNodes;
	QVector< RoutingDaemonEdge > pathEdges;
	QStringList nameStrings;
	QStringList typeStrings;

	void post( QIODevice* out )
	{
		QByteArray buffer;
		QDataStream stream( &buffer, QIODevice::WriteOnly );
		stream << qint32( type );
		stream << seconds;
		stream << pathNodes;
		stream << pathEdges;
		stream << nameStrings;
		stream << typeStrings;
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
		stream >> pathNodes;
		stream >> pathEdges;
		stream >> nameStrings;
		stream >> typeStrings;

		return true;
	}

};

#endif // SIGNALS_H
