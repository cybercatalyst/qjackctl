// qjackctlSession.h
//
/****************************************************************************
   Copyright (C) 2003-2010, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

*****************************************************************************/

#ifndef __qjackctlSession_h
#define __qjackctlSession_h

#include <QString>
#include <QHash>

#include <jack/jack.h>


//----------------------------------------------------------------------------
// qjackctlSession -- JACK session container.

class qjackctlSession
{
public:

	// Constructor.
	qjackctlSession(jack_client_t *pJackClient, const QString& sSessionDir);

	// Destructor.
	~qjackctlSession();

	// Container structs.
	struct ConnectItem
	{
		ConnectItem() : connected(false) {}

		QString client_name;
		QString port_name;
		bool    connected;
	};

	typedef QList<ConnectItem *> ConnectList;

	struct PortItem
	{
		PortItem() : connected(0) {}
		~PortItem() { qDeleteAll(connects); }

		QString     port_name;
		int         connected;
		ConnectList connects;
	};

	typedef QList<PortItem *> PortList;

	struct ClientItem
	{
		ClientItem() : connected(0) {}
		~ClientItem() { qDeleteAll(ports); }

		QString  client_name;
		QString  client_uuid;
		QString  client_command;
		int      connected;
		PortList ports;
	};

	typedef QHash<QString, ClientItem *> ClientList;

	// Client list accessor (read-only)
	const ClientList& clients() const;

	// House-keeper.
	void clear();

	// Critical methods.
	bool save(int iSessionType = 0);
	bool load();

	// Update (re)connections utility method.
	bool update();

protected:

	// File methods.
	bool loadFile(const QString& sFilename);
	bool saveFile(const QString& sFilename);

private:

	// Instance variables.
	jack_client_t *m_pJackClient;

	QString m_sSessionDir;

	ClientList m_clients;	
};


#endif	// __qjackctlSession_h

// end of qjackctlSession.h
