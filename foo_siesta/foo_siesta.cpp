#include "stdafx.h"
#include "SDK/foobar2000.h"
#include "helpers/helpers.h"
#include "Server.h"

#define COMPONENT_NAME "foo_siesta"
#define COMPONENT_DESC "RESTful web service"
#define FOO_CONSOLE console::formatter() << "RESTful: "

//#include "server.h"
//#include "config.h"

pfc::string_formatter about() {
	pfc::string_formatter about;
	about
		<< "compiled:\n"
		<< __DATE__ << ", " << __TIME__ << "\n"
		<< "\n"
		<< "written by:\n"
		<< "Ale Becher (http://aleciten.github.io)\n"
		<< "\n"
		<< "thanks to\n"
		<< "Holger Stenger (http://foosion.foobar2000.org/) for the much needed SDK tutorial.\n"
		<< "\n"
		<< "libs:\n"
		<< "POCO NET C++ libraries (http://http://pocoproject.org/)";

	return about;
}

DECLARE_COMPONENT_VERSION_COPY(
	COMPONENT_DESC,
	"0.1",
	about()	
);

/*
	/<component>/<action>

	GET /state -> Overall state (playback status, track, time)

	GET /playlist         -> List of playlists
	GET /playlist/active  -> Active playlist + contents
	GET /playlist/{index} -> Playlist contents
	DEL /playlist/active  -> Remove the active playlist
	DEL /playlist/{index} -> Remove playlist by index

	POST /control/play          -> Play
	POST /control/pause         -> Pause
	POST /control/stop          -> Stop
	POST /control/shuffle       -> Shuffle
	POST /control/next          -> Shuffle
	POST /control/previous      -> Shuffle
	GET  /control/order         -> Get ordering
	POST /control/order/<order> -> Set order
	GET  /control/vol           -> Get volume
	POST /control/vol/<vol>     -> Set volume
*/


//std::unique_ptr<server> m_server;

std::unique_ptr<Server> g_server;
class initquit_foo_webservice : public initquit
{
public:
	virtual void on_init(){
		g_server.reset(new Server(999, "/foo"));
		g_server->Start();
	}

	virtual void on_quit() {
		g_server->Stop();
	}
};
initquit_factory_t<initquit_foo_webservice> foo_initquit;


