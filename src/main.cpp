// main.cpp
//
/****************************************************************************
   Copyright (C) 2003-2008, rncbc aka Rui Nuno Capela. All rights reserved.

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

#include "qjackctlAbout.h"
#include "qjackctlSetup.h"
#include "qjackctlMainForm.h"

#include <QApplication>
#include <QTranslator>
#include <QLocale>


#if defined(Q_WS_X11)

//-------------------------------------------------------------------------
// Single application instance stuff (Qt/X11 only atm.)
//

#include <QX11Info>

#include <X11/Xatom.h>
#include <X11/Xlib.h>

#define QJACKCTL_XUNIQUE "qjackctlMainForm_xunique"

#endif

class qjackctlApplication : public QApplication
{
public:

	// Constructor.
	qjackctlApplication(int& argc, char **argv)
		: QApplication(argc, argv), m_pWidget(0)
	{
	#if defined(Q_WS_X11)
		m_pDisplay = QX11Info::display();
		m_aUnique  = XInternAtom(m_pDisplay, QJACKCTL_XUNIQUE, false);
		XGrabServer(m_pDisplay);
		m_wOwner = XGetSelectionOwner(m_pDisplay, m_aUnique);
		XUngrabServer(m_pDisplay);
	#endif
	}

	// Main application widget accessors.
	void setMainWidget(QWidget *pWidget)
	{
		m_pWidget = pWidget;
	#if defined(Q_WS_X11)
		XGrabServer(m_pDisplay);
		m_wOwner = m_pWidget->winId();
		XSetSelectionOwner(m_pDisplay, m_aUnique, m_wOwner, CurrentTime);
		XUngrabServer(m_pDisplay);
	#endif
	}

	QWidget *mainWidget() const { return m_pWidget; }

	// Check if another instance is running,
    // and raise its proper main widget...
	bool setup()
	{
	#if defined(Q_WS_X11)
		if (m_wOwner != None) {
			// First, notify any freedesktop.org WM
			// that we're about to show the main widget...
			Screen *pScreen = XDefaultScreenOfDisplay(m_pDisplay);
			int iScreen = XScreenNumberOfScreen(pScreen);
			XEvent ev;
			memset(&ev, 0, sizeof(ev));
			ev.xclient.type = ClientMessage;
			ev.xclient.display = m_pDisplay;
			ev.xclient.window = m_wOwner;
			ev.xclient.message_type = XInternAtom(m_pDisplay, "_NET_ACTIVE_WINDOW", false);
			ev.xclient.format = 32;
			ev.xclient.data.l[0] = 0; // Source indication.
			ev.xclient.data.l[1] = 0; // Timestamp.
			ev.xclient.data.l[2] = 0; // Requestor's currently active window (none)
			ev.xclient.data.l[3] = 0;
			ev.xclient.data.l[4] = 0;
			XSelectInput(m_pDisplay, m_wOwner, StructureNotifyMask);
			XSendEvent(m_pDisplay, RootWindow(m_pDisplay, iScreen), false,
				(SubstructureNotifyMask | SubstructureRedirectMask), &ev);
			XSync(m_pDisplay, false);
			XRaiseWindow(m_pDisplay, m_wOwner);
			// And then, let it get caught on destination
			// by QApplication::x11EventFilter...
			QByteArray value = QJACKCTL_XUNIQUE;
			XChangeProperty(
				m_pDisplay,
				m_wOwner,
				m_aUnique,
				m_aUnique, 8,
				PropModeReplace,
				(unsigned char *) value.data(),
				value.length());
			// Done.
			return true;
		}
	#endif
		return false;
	}

#if defined(Q_WS_X11)
	bool x11EventFilter(XEvent *pEv)
	{
		if (m_pWidget && m_wOwner != None
			&& pEv->type == PropertyNotify
			&& pEv->xproperty.window == m_wOwner
			&& pEv->xproperty.state == PropertyNewValue) {
			// Always check whether our property-flag is still around...
			Atom aType;
			int iFormat = 0;
			unsigned long iItems = 0;
			unsigned long iAfter = 0;
			unsigned char *pData = 0;
			if (XGetWindowProperty(
					m_pDisplay,
					m_wOwner,
					m_aUnique,
					0, 1024,
					false,
					m_aUnique,
					&aType,
					&iFormat,
					&iItems,
					&iAfter,
					&pData) == Success
				&& aType == m_aUnique && iItems > 0 && iAfter == 0) {
				// Avoid repeating it-self...
				XDeleteProperty(m_pDisplay, m_wOwner, m_aUnique);
				// Just make it always shows up fine...
				m_pWidget->show();
				m_pWidget->raise();
				m_pWidget->activateWindow();
			}
			// Free any left-overs...
			if (iItems > 0 && pData)
				XFree(pData);
		}
		return QApplication::x11EventFilter(pEv);
	}
#endif
	
private:

	QWidget *m_pWidget;

#if defined(Q_WS_X11)
	Display *m_pDisplay;
	Atom     m_aUnique;
	Window   m_wOwner;
#endif
};


//-------------------------------------------------------------------------
// main - The main program trunk.
//

int main ( int argc, char **argv )
{
	qjackctlApplication app(argc, argv);

	// Load translation support.
	QTranslator translator(0);
	QLocale loc;
	if (loc.language() != QLocale::C) {
		QString sLocName = "qjackctl_" + loc.name();
		if (!translator.load(sLocName, ".")) {
			QString sLocPath = CONFIG_PREFIX "/share/locale";
			if (!translator.load(sLocName, sLocPath))
				fprintf(stderr, "Warning: no locale found: %s/%s.qm\n",
					sLocPath.toUtf8().constData(),
					sLocName.toUtf8().constData());
		}
		app.installTranslator(&translator);
	}

	// Construct default settings; override with command line arguments.
	qjackctlSetup settings;
	if (!settings.parse_args(app.argc(), app.argv())) {
		app.quit();
		return 1;
	}

	// Check if we'll just start an external program...
	if (!settings.sCmdLine.isEmpty()) {
		jack_client_t *pJackClient
			= jack_client_open("qjackctl-start", JackNoStartServer, NULL);
		if (pJackClient) {
			jack_client_close(pJackClient);
			int iExitStatus = ::system(settings.sCmdLine.toUtf8().constData());
			app.quit();
			return iExitStatus;
		}
	}

	// Have another instance running?
	if (app.setup()) {
		app.quit();
		return 2;
	}

	// What style do we create these forms?
	Qt::WindowFlags wflags = Qt::Window
#if QT_VERSION >= 0x040200
		| Qt::CustomizeWindowHint
#endif
		| Qt::WindowTitleHint
		| Qt::WindowSystemMenuHint
		| Qt::WindowMinMaxButtonsHint;
	if (settings.bKeepOnTop)
		wflags |= Qt::Tool;
	// Construct the main form, and show it to the world.
	qjackctlMainForm w(0, wflags);
//	app.setMainWidget(&w);
	w.setup(&settings);
	// If we have a systray icon, we'll skip this.
	if (!settings.bSystemTray) {
		w.show();
		w.adjustSize();
	}

	// Settle this one as application main widget...
	app.setMainWidget(&w);

	// Register the quit signal/slot.
	// app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
	app.setQuitOnLastWindowClosed(false);

	return app.exec();
}

// end of main.cpp
