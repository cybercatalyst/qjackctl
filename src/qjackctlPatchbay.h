// qjackctlPatchbay.h
//
/****************************************************************************
   Copyright (C) 2003-2004, rncbc aka Rui Nuno Capela. All rights reserved.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

*****************************************************************************/

#ifndef __qjackctlPatchbay_h
#define __qjackctlPatchbay_h

#include <qdragobject.h>
#include <qlistview.h>
#include <qheader.h>
#include <qsplitter.h>
#include <qptrlist.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qpopupmenu.h>

// Our external patchbay models.
#include "qjackctlPatchbayRack.h"

// QListViewItem::rtti return values.
#define QJACKCTL_SOCKETITEM 2001
#define QJACKCTL_PLUGITEM   2002

// Forward declarations.
class qjackctlPlugItem;
class qjackctlSocketItem;
class qjackctlSocketList;
class qjackctlSocketListView;
class qjackctlPatchworkView;
class qjackctlPatchbayView;
class qjackctlPatchbay;

// Pixmap-set indexes.
#define QJACKCTL_XPM_AUDIO_SOCKET   0
#define QJACKCTL_XPM_AUDIO_SOCKET_X 1
#define QJACKCTL_XPM_AUDIO_PLUG     2
#define QJACKCTL_XPM_MIDI_SOCKET    3
#define QJACKCTL_XPM_MIDI_SOCKET_X  4
#define QJACKCTL_XPM_MIDI_PLUG      5
#define QJACKCTL_XPM_PIXMAPS        6


// Patchbay plug (port) list item.
class qjackctlPlugItem : public QListViewItem
{
public:

    // Constructor.
    qjackctlPlugItem(qjackctlSocketItem *pSocket, const QString& sPlugName, qjackctlPlugItem *pPlugAfter);
    // Default destructor.
    ~qjackctlPlugItem();

    // Instance accessors.
    const QString& socketName();
    const QString& plugName();

    // Patchbay socket item accessor.
    qjackctlSocketItem *socket();

    // To virtually distinguish between list view items.
    virtual int rtti() const;

private:

    // Instance variables.
    qjackctlSocketItem *m_pSocket;
    QString m_sPlugName;
};


// Patchbay socket (client) list item.
class qjackctlSocketItem : public QListViewItem
{
public:

    // Constructor.
    qjackctlSocketItem(qjackctlSocketList *pSocketList, const QString& sSocketName, const QString& sClientName, int iSocketType, bool bExclusive, qjackctlSocketItem *pSocketAfter);
    // Default destructor.
    ~qjackctlSocketItem();

    // Instance accessors.
    const QString& socketName();
    const QString& clientName();
    int socketType();
    bool isExclusive();

    void setSocketName (const QString& sSocketName);
    void setClientName (const QString& sClientName);
    void setSocketType (int iSocketType);
    void setExclusive  (bool bExclusive);

    // Socket flags accessor.
    bool isReadable();
    
    // Connected plug list primitives.
    void addConnect(qjackctlSocketItem *pSocket);
    void removeConnect(qjackctlSocketItem *pSocket);

    // Connected plug  finders.
    qjackctlSocketItem *findConnectPtr(qjackctlSocketItem *pSocketPtr);
    
    // Connection list accessor.
    QPtrList<qjackctlSocketItem>& connects();

    // Plug list primitive methods.
    void addPlug(qjackctlPlugItem *pPlug);
    void removePlug(qjackctlPlugItem *pPlug);

    // Plug finder.
    qjackctlPlugItem *findPlug(const QString& sPlugName);

    // Plug list accessor.
    QPtrList<qjackctlPlugItem>& plugs();

    // To virtually distinguish between list view items.
    int rtti() const;

    // Plug list cleaner.
    void clear();

    // Retrieve a context pixmap.
    QPixmap& pixmap(int iPixmap);
    
    // Update pixmap to its proper context.
    void updatePixmap();

private:

    // Instance variables.
    qjackctlSocketList *m_pSocketList;
    QString m_sSocketName;
    QString m_sClientName;
    int m_iSocketType;
    bool m_bExclusive;

    // Plug (port) list.
    QPtrList<qjackctlPlugItem> m_plugs;

    // Connection cache list.
    QPtrList<qjackctlSocketItem> m_connects;
};


// Patchbay socket (client) list.
class qjackctlSocketList : public QObject
{
    Q_OBJECT

public:

    // Constructor.
    qjackctlSocketList(qjackctlSocketListView *pListView, bool bReadable);
    // Default destructor.
    ~qjackctlSocketList();

    // Socket list primitive methods.
    void addSocket(qjackctlSocketItem *pSocket);
    void removeSocket(qjackctlSocketItem *pSocket);

    // Socket finder.
    qjackctlSocketItem *findSocket(const QString& sSocketName);

    // List view accessor.
    qjackctlSocketListView *listView();

    // Socket flags accessor.
    bool isReadable();

    // Socket aesthetics accessors.
    QString& socketCaption();

    // JACK client accessors.
    void setJackClient(jack_client_t *pJackClient);
    jack_client_t *jackClient();

    // ALSA sequencer accessors.
    void setAlsaSeq(snd_seq_t *pAlsaSeq);
    snd_seq_t *alsaSeq();

    // Socket list cleaner.
    void clear();

    // Client:port snapshot.
    void clientPortsSnapshot();

    // Socket list accessor.
    QPtrList<qjackctlSocketItem>& sockets();

    // Find the current selected socket item in list.
    qjackctlSocketItem *selectedSocketItem();

    // Retrieve a context pixmap.
    QPixmap& pixmap(int iPixmap);

    // Escape and format a string as a regular expresion
    static QString escapeRegExpDigits(const QString& s, unsigned int iThreshold = 2);

public slots:

    // Socket item interactivity methods.
    bool addSocketItem();
    bool removeSocketItem();
    bool editSocketItem();
    bool exclusiveSocketItem();
    bool moveUpSocketItem();
    bool moveDownSocketItem();

private:

    // Merge two pixmaps with union of respective masks.
    QPixmap *createPixmapMerge(const QPixmap& xpmDst, const QPixmap& xpmSrc);

    // Instance variables.
    qjackctlSocketListView *m_pListView;
    bool m_bReadable;
    QString  m_sSocketCaption;
    jack_client_t *m_pJackClient;
    snd_seq_t *m_pAlsaSeq;

    QPixmap *m_apPixmaps[QJACKCTL_XPM_PIXMAPS];

    QPtrList<qjackctlSocketItem> m_sockets;
};


//----------------------------------------------------------------------------
// qjackctlSocketListView -- Socket list view, supporting drag-n-drop.

class qjackctlSocketListView : public QListView
{
    Q_OBJECT

public:

    // Constructor.
    qjackctlSocketListView(qjackctlPatchbayView *pPatchbayView, bool bReadable);
    // Default destructor.
    ~qjackctlSocketListView();

    // Patchbay dirty flag accessors.
    void setDirty (bool bDirty);
    bool dirty();
    
    // Auto-open timer methods.
    void setAutoOpenTimeout(int iAutoOpenTimeout);
    int autoOpenTimeout();

protected slots:

    // Auto-open timeout slot.
    void timeoutSlot();

protected:

    // Drag-n-drop stuff -- reimplemented virtual methods.
    virtual void dragEnterEvent(QDragEnterEvent *pDragEnterEvent);
    virtual void dragMoveEvent(QDragMoveEvent *pDragMoveEvent);
    virtual void dragLeaveEvent(QDragLeaveEvent *);
    virtual void dropEvent(QDropEvent *pDropEvent);
    virtual QDragObject *dragObject();
    // Context menu request event handler.
    virtual void contextMenuEvent(QContextMenuEvent *);

private:

    // Bindings.
    qjackctlPatchbayView *m_pPatchbayView;
    bool m_bReadable;

    // Drag-n-drop stuff.
    QListViewItem *dragDropItem(const QPoint& epos);

    // Auto-open timer.
    int    m_iAutoOpenTimeout;
    QTimer *m_pAutoOpenTimer;
    // Item we'll eventually drop something.
    QListViewItem *m_pDragDropItem;
};


//----------------------------------------------------------------------------
// qjackctlPatchworkView -- Socket plugging connector widget.

class qjackctlPatchworkView : public QWidget
{
    Q_OBJECT

public:

    // Constructor.
    qjackctlPatchworkView(qjackctlPatchbayView *pPatchbayView);
    // Default destructor.
    ~qjackctlPatchworkView();

public slots:

    // Useful slots (should this be protected?).
    void listViewChanged(QListViewItem *);
    void contentsMoved(int, int);

protected:

    // Specific event handlers.
    virtual void paintEvent(QPaintEvent *);
    virtual void resizeEvent(QResizeEvent *);
    // Context menu request event handler.
    virtual void contextMenuEvent(QContextMenuEvent *);
    
private:

    // Drawing methods.
    void drawConnectionLine(QPainter& p, int x1, int y1, int x2, int y2, int h1, int h2);
    void drawConnections();
    
    // Local instance variables.
    qjackctlPatchbayView *m_pPatchbayView;
};


//----------------------------------------------------------------------------
// qjackctlPatchbayView -- Patchbay integrated widget.

class qjackctlPatchbayView : public QSplitter
{
    Q_OBJECT

public:

    // Constructor.
    qjackctlPatchbayView(QWidget *pParent = 0, const char *pszName = 0);
    // Default destructor.
    ~qjackctlPatchbayView();

    // Widget accesors.
    qjackctlSocketListView *OListView()     { return m_pOListView; }
    qjackctlSocketListView *IListView()     { return m_pIListView; }
    qjackctlPatchworkView  *PatchworkView() { return m_pPatchworkView; }

    // Patchbay object binding methods.
    void setBinding(qjackctlPatchbay *pPatchbay);
    qjackctlPatchbay *binding();
    
    // Socket list accessors.
    qjackctlSocketList *OSocketList();
    qjackctlSocketList *ISocketList();

    // Patchbay dirty flag accessors.
    void setDirty (bool bDirty);
    bool dirty();

signals:

    // Contents change signal.
    void contentsChanged();

public slots:

    // Common context menu slot.
    void contextMenu(const QPoint& pos, qjackctlSocketList *pSocketList);
    
private:

    // Child controls.
    qjackctlSocketListView *m_pOListView;
    qjackctlSocketListView *m_pIListView;
    qjackctlPatchworkView  *m_pPatchworkView;

    // The main binding object.
    qjackctlPatchbay *m_pPatchbay;
    
    // The obnoxious dirty flag.
    bool m_bDirty;
};


//----------------------------------------------------------------------------
// qjackctlPatchbay -- Patchbay integrated object.

class qjackctlPatchbay : public QObject
{
    Q_OBJECT

public:

    // Constructor.
    qjackctlPatchbay(qjackctlPatchbayView *pPatchbayView);
    // Default destructor.
    ~qjackctlPatchbay();

    // Explicit connection tests.
    bool canConnectSelected();
    bool canDisconnectSelected();
    bool canDisconnectAll();

    // Socket list accessors.
    qjackctlSocketList *OSocketList();
    qjackctlSocketList *ISocketList();

    // External rack transfer methods.
    void loadRack(qjackctlPatchbayRack *pPatchbayRack);
    void saveRack(qjackctlPatchbayRack *pPatchbayRack);

    // JACK client property accessors.
    void setJackClient(jack_client_t *pJackClient);
    jack_client_t *jackClient();

    // ALSA sequencer property accessors.
    void setAlsaSeq(snd_seq_t *pAlsaSeq);
    snd_seq_t *alsaSeq();

public slots:

    // Complete contents refreshner.
    void refresh();

    // Explicit connection slots.
    bool connectSelected();
    bool disconnectSelected();
    bool disconnectAll();

    // Complete patchbay clearer.
    void clear();

    // Do actual and complete connections snapshot.
    void connectionsSnapshot();

private:

    // Internal rack transfer methods.
    void loadRackSockets (qjackctlSocketList *pSocketList, QPtrList<qjackctlPatchbaySocket>& socketlist);
    void saveRackSockets (qjackctlSocketList *pSocketList, QPtrList<qjackctlPatchbaySocket>& socketlist);

    // Connect/Disconnection primitives.
    void connectSockets(qjackctlSocketItem *pOSocket, qjackctlSocketItem *pISocket);
    void disconnectSockets(qjackctlSocketItem *pOSocket, qjackctlSocketItem *pISocket);

    // Output socket-plug connection snapshot subroutines.
    void socketPlugAudioSnapshot(qjackctlSocketItem *pOSocket, qjackctlPlugItem *pOPlug);
    void socketPlugMidiSnapshot(qjackctlSocketItem *pOSocket, qjackctlPlugItem *pOPlug);

    // Instance variables.
    qjackctlPatchbayView *m_pPatchbayView;
    qjackctlSocketList *m_pOSocketList;
    qjackctlSocketList *m_pISocketList;
};


#endif  // __qjackctlPatchbay_h

// end of qjackctlPatchbay.h
