/****************************************************************************
   Copyright (C) 2015, Jacob Dawid <jacob@omg-it.works>

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

#pragma once

// Qt includes
#include <QTreeWidget>

// Own includes
class ConnectionsSplitter;
#include "connectalias.h"

class ClientListTreeWidget : public QTreeWidget {
    Q_OBJECT

public:
    ClientListTreeWidget(ConnectionsSplitter *pConnectView, bool bReadable);
    ~ClientListTreeWidget();

    // Auto-open timer methods.
    void setAutoOpenTimeout(int iAutoOpenTimeout);
    int autoOpenTimeout() const;

    // Aliasing support methods.
    void setAliases(ConnectAlias *pAliases, bool bRenameEnabled);
    ConnectAlias *aliases() const;
    bool renameEnabled() const;

protected slots:

    // In-place aliasing slots.
    void startRenameSlot();
    void renamedSlot();
    // Auto-open timeout slot.
    void timeoutSlot();

protected:

    // Trap for help/tool-tip events.
    bool eventFilter(QObject *pObject, QEvent *pEvent);

    // Drag-n-drop stuff.
    QTreeWidgetItem *dragDropItem(const QPoint& pos);

    // Drag-n-drop stuff -- reimplemented virtual methods.
    void dragEnterEvent(QDragEnterEvent *pDragEnterEvent);
    void dragMoveEvent(QDragMoveEvent *pDragMoveEvent);
    void dragLeaveEvent(QDragLeaveEvent *);
    void dropEvent(QDropEvent *pDropEvent);

    // Handle mouse events for drag-and-drop stuff.
    void mousePressEvent(QMouseEvent *pMouseEvent);
    void mouseMoveEvent(QMouseEvent *pMouseEvent);

    // Context menu request event handler.
    void contextMenuEvent(QContextMenuEvent *);

private:

    // Bindings.
    ConnectionsSplitter *m_pConnectView;

    // Auto-open timer.
    int     m_iAutoOpenTimeout;
    QTimer *m_pAutoOpenTimer;

    // Items we'll eventually drop something.
    QTreeWidgetItem *m_pDragItem;
    QTreeWidgetItem *m_pDropItem;
    // The point from where drag started.
    QPoint m_posDrag;

    // Aliasing support.
    ConnectAlias *m_pAliases;
    bool m_bRenameEnabled;
};
