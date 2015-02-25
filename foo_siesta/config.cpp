#include "stdafx.h"
#include "config.h"


static const GUID guid_g_advconfig_server_url = { 0xda9f4b8b, 0xcfc4, 0x424e, { 0xb0, 0x85, 0xf3, 0xc1, 0x23, 0x26, 0xe6, 0xb8 } };
advconfig_string_factory g_advconfig_server_url(
	// display name
	COMPONENT_NAME " server url",

	// GUID of our setting
	guid_g_advconfig_server_url,

	// GUID of the parent branch
	advconfig_branch::guid_branch_display,

	// sorting priority (we leave it at 0.0)
	0.0,

	// initial value
	"/foobar2000"
);
