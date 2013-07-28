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

#include <QString>
#include <QVector>
#include <QDataStream>
#include <QStringList>
#include <QLocalSocket>

namespace MoNav {

	// has to be send before each command to identify the following command type
	struct CommandType {
		enum Type{
			RoutingCommand = 0,
			UnpackCommand = 1
		} value;

		void post( QIODevice* out )
		{
			qint32 temp = value;
			out->write( ( const char* ) &temp, sizeof( qint32 ) );
		}

		bool read( QLocalSocket* in )
		{
			while ( in->bytesAvailable() < ( int ) sizeof( qint32 ) ) {
				if ( in->state() != QLocalSocket::ConnectedState )
					return false;
				in->waitForReadyRead( 100 );
			}

			qint32 temp;
			in->read( ( char* ) &temp, sizeof( qint32 ) );
			value = ( Type ) temp;

			return true;
		}
	};

	struct Node {
		double latitude;
		double longitude;

		friend QDataStream& operator<< ( QDataStream& out, const Node& node )
		{
			out << node.latitude;
			out << node.longitude;
			return out;
		}

		friend QDataStream& operator>> ( QDataStream& in, Node& node )
		{
			in >> node.latitude;
			in >> node.longitude;
			return in;
		}
	};

	struct Edge {
		unsigned length; // length of the edge == number of edges it represents == number of nodes - 1
		unsigned name; // name ID of the edge
		unsigned type; // type ID of the edge
		unsigned seconds; // travel time metric for the edge
		bool branchingPossible; // is it possible to choose between more than one subsequent edge ( turning around on bidirectional edges does not count )

		friend QDataStream& operator<< ( QDataStream& out, const Edge& edge )
		{
			out << edge.length;
			out << edge.name;
			out << edge.type;
			out << edge.seconds;
			out << edge.branchingPossible;
			return out;
		}

		friend QDataStream& operator>> ( QDataStream& in, Edge& edge )
		{
			in >> edge.length;
			in >> edge.name;
			in >> edge.type;
			in >> edge.seconds;
			in >> edge.branchingPossible;
			return in;
		}
	};

	class RoutingCommand {

	public:

		RoutingCommand()
		{
			lookupRadius = 10000; // 10km should suffice for most applications
			lookupStrings = false;
		}

		// waypoint edge lookup radius in meters
		double lookupRadius;
		// lookup street name / type strings?
		bool lookupStrings;
		// a valid  routing module directory
		QString dataDirectory;
		// waypoints of the route
		QVector< Node > waypoints;

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

	class RoutingResult {

	public:

		enum ResultType {
			LoadFailed = 1, RouteFailed = 2, NameLookupFailed = 3, TypeLookupFailed = 4, Success = 5
		} type;

		double seconds;
		QVector< Node > pathNodes;
		QVector< Edge > pathEdges;
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

	class UnpackCommand
	{
	public:

		UnpackCommand()
		{
			deleteFile = false;
		}

		// MoNav Map Module file to be unpacked
		// it will be unpacked in the directory of the same name
		// e.g. test.mmm -> test/
		QString mapModuleFile;
		// delete file after unpacking?
		bool deleteFile;

		void post( QIODevice* out )
		{
			QByteArray buffer;
			QDataStream stream( &buffer, QIODevice::WriteOnly );
			stream << mapModuleFile;
			stream << deleteFile;
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
			stream >> mapModuleFile;
			stream >> deleteFile;

			return true;
		}
	};

	class UnpackResult
	{
	public:

		enum ResultType {
			FailUnpacking = 1, Success = 2
		} type;

		void post( QIODevice* out )
		{
			qint32 temp = type;
			out->write( ( const char* ) &temp, sizeof( qint32 ) );
		}

		bool read( QLocalSocket* in )
		{
			while ( in->bytesAvailable() < ( int ) sizeof( qint32 ) ) {
				if ( in->state() != QLocalSocket::ConnectedState )
					return false;
				in->waitForReadyRead( 100 );
			}

			qint32 temp;
			in->read( ( char* ) &temp, sizeof( qint32 ) );
			type = ResultType( temp );

			return true;
		}
	};

}

#endif // SIGNALS_H
