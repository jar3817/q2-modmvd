/*
Copyright (C) 2000 Shane Powell

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

//
// q2admin
//
// g_main.c
//
// copyright 2000 Shane Powell
//

#include "g_local.h"

#ifdef __GNUC__
#include <dlfcn.h>
#elif defined(WIN32)
#include <windows.h>
#endif

#ifdef __GNUC__
void *hdll = NULL;

#ifdef LINUXAXP
	#define DLLNAME   "gameaxp.real.so"
#elif defined(SOLARIS_INTEL)
	#define DLLNAME   "gamei386.real.so"
#elif defined(SOLARIS_SPARC)
	#define DLLNAME   "gamesparc.real.so"
#elif defined (LINUX) && defined (__x86_64__)
	#define DLLNAME "gamex86_64.real.so"
#elif defined (LINUX) && defined (i386)
	#define DLLNAME "gamei386.real.so"
#else
	#error Unknown GNUC OS
#endif

#elif defined(WIN32)
HINSTANCE hdll;
#define DLLNAME   "gamex86.dll"
#define DLLNAMEMODDIR "gamex86.real.dll"
#else
#error Unknown OS
#endif

typedef game_export_t  *GAMEAPI (game_import_t *import);

qboolean soloadlazy;

void MVD_ShutdownGame (void)
{
	if(!dllloaded) return;


	dllglobals->Shutdown();
	
		
#ifdef __GNUC__
	dlclose(hdll);
#elif defined(WIN32)
	FreeLibrary(hdll);
#endif
	
	dllloaded = FALSE;
	
}

void MVD_SpawnEntities (const char *mapname, const char *entities, const char *spawnpoint);
void MVD_ClientThink (edict_t *ent, usercmd_t *cmd);
qboolean MVD_ClientConnect (edict_t *ent, char *userinfo);
void MVD_ClientUserinfoChanged (edict_t *ent, char *userinfo);
void MVD_ClientDisconnect (edict_t *ent);
void MVD_ClientBegin (edict_t *ent);
void MVD_ClientCommand (edict_t *ent);
void MVD_WriteGame (const char *filename, qboolean autosave);
void MVD_ReadGame (const char *filename);
void MVD_WriteLevel (const char *filename);
void MVD_ReadLevel (const char *filename);
void MVD_InitGame (void);
void MVD_RunFrame (void);
void MVD_ServerCommand (void);

/*
=================
GetGameAPI
 
Returns a pointer to the structure with all entry points
and global variables
=================
*/
game_export_t *GetGameAPI(game_import_t *import)
{
	GAMEAPI *getapi;
#ifdef __GNUC__
	int loadtype;
#endif
	char dllname[256];
	unsigned int i;// UPDATE
	dllloaded = FALSE;
	
//	import->bprintf = bprintf_internal;
//	import->cprintf = cprintf_internal;
//	import->dprintf = dprintf_internal;
//	import->AddCommandString = AddCommandString_internal;
//	//import->Pmove = Pmove_internal;
//	import->linkentity = linkentity_internal;
//	import->unlinkentity = unlinkentity_internal;
	
	

	gi = *import;
	globals.Init = MVD_InitGame;
	globals.Shutdown = MVD_ShutdownGame;
	globals.SpawnEntities = MVD_SpawnEntities;
	globals.WriteGame = MVD_WriteGame;
	globals.ReadGame = MVD_ReadGame;
	globals.WriteLevel = MVD_WriteLevel;
	globals.ReadLevel = MVD_ReadLevel;	
	globals.ClientThink = MVD_ClientThink;
	globals.ClientConnect = MVD_ClientConnect;
	globals.ClientUserinfoChanged = MVD_ClientUserinfoChanged;
	globals.ClientDisconnect = MVD_ClientDisconnect;
	globals.ClientBegin = MVD_ClientBegin;
	globals.ClientCommand = MVD_ClientCommand;	
	globals.RunFrame = MVD_RunFrame;
	globals.ServerCommand = MVD_ServerCommand;

	cvar_t *serverbinip, *port, *rcon_password, *gamedir;
	serverbindip = gi.cvar("ip", "", 0);
	port = gi.cvar("port", "", 0);
	rcon_password = gi.cvar("rcon_password", "", 0) ; // UPDATE
	gamedir = gi.cvar ("game", "baseq2", 0);
        q2a_strcpy(moddir, gamedir->string);	
	
#ifdef __GNUC__
	loadtype = soloadlazy ? RTLD_LAZY : RTLD_NOW;
	sprintf(dllname, "%s/%s", moddir, DLLNAME);
	hdll = dlopen(dllname, loadtype);
#elif defined(WIN32)
	if(quake2dirsupport)
		{
			sprintf(dllname, "%s/%s", moddir, DLLNAME);
		}
	else
		{
			sprintf(dllname, "%s/%s", moddir, DLLNAMEMODDIR);
		}
	
	hdll = LoadLibrary(dllname);
#endif
	
	if(hdll == NULL)
		{
			// try the baseq2 directory...
			sprintf(dllname, "baseq2/%s", DLLNAME);
			
#ifdef __GNUC__
			hdll = dlopen(dllname, loadtype);
#elif defined(WIN32)
			hdll = LoadLibrary(dllname);
#endif
			
#ifdef __GNUC__
			sprintf(dllname, "%s/%s", moddir, DLLNAME);
#elif defined(WIN32)
			if(quake2dirsupport)
				{
					sprintf(dllname, "%s/%s", moddir, DLLNAME);
				}
			else
				{
					sprintf(dllname, "%s/%s", moddir, DLLNAMEMODDIR);
				}
#endif
			
			if(hdll == NULL)
				{
					gi.dprintf ("Unable to load DLL %s.\n", dllname);
					return &globals;
				}
			else
				{
					gi.dprintf ("Unable to load DLL %s, loading baseq2 DLL.\n", dllname);
				}
		}
		
#ifdef __GNUC__
	getapi = (GAMEAPI *)dlsym(hdll, "GetGameAPI");
#elif defined(WIN32)
	getapi = (GAMEAPI *)GetProcAddress (hdll, "GetGameAPI");
#endif
	
	if(getapi == NULL)
		{
#ifdef __GNUC__
			dlclose(hdll);
#elif defined(WIN32)
			FreeLibrary(hdll);
#endif
			
			gi.dprintf ("No \"GetGameApi\" entry in DLL %s.\n", dllname);
			return &globals;
		}
		
	dllglobals = (*getapi)(import);
	dllloaded = TRUE;
	copyDllInfo();
	import->cprintf = gi.cprintf;
	cvar_t *mvd_ip, *mvd_port;
	mvd_ip = gi.cvar("mvd_ip", "", 0);
	mvd_port = gi.cvar("mvd_port","",0);
	gi.dprintf("Loading Multi-View Demo broadcasting module, gtv host: %s:%s\n",mvd_ip->string, mvd_port->string);	
	return &globals;
}


// passthrough
void MVD_SpawnEntities (const char *mapname, const char *entities, const char *spawnpoint)
{
	dllglobals->SpawnEntities(mapname, entities, spawnpoint);
	copyDllInfo();
}

void MVD_ClientThink (edict_t *ent, usercmd_t *cmd)
{
	dllglobals->ClientThink(ent,cmd);
	copyDllInfo();
}

qboolean MVD_ClientConnect (edict_t *ent, char *userinfo)
{
	dllglobals->ClientConnect(ent, userinfo);
	copyDllInfo();
}

void MVD_ClientUserinfoChanged (edict_t *ent, char *userinfo)
{
	dllglobals->ClientUserinfoChanged(ent, userinfo);
	copyDllInfo();
}

void MVD_ClientDisconnect (edict_t *ent)
{
	dllglobals->ClientDisconnect(ent);
	copyDllInfo();
}

void MVD_ClientBegin (edict_t *ent)
{
	dllglobals->ClientBegin(ent);
	copyDllInfo();
}

void MVD_ClientCommand (edict_t *ent)
{
	dllglobals->ClientCommand(ent);
	copyDllInfo();
}

/*void MVD_RunEntity (edict_t *ent)
{
	dllglobals->RunEntity(ent);
}*/

void MVD_WriteGame (const char *filename, qboolean autosave)
{
	dllglobals->WriteGame(filename, autosave);
	copyDllInfo();
}
	
void MVD_ReadGame (const char *filename)
{
	dllglobals->ReadGame(filename);
	copyDllInfo();
}

void MVD_WriteLevel (const char *filename)
{
	dllglobals->WriteLevel(filename);
	copyDllInfo();
}

void MVD_ReadLevel (const char *filename)
{
	dllglobals->ReadLevel(filename);
	copyDllInfo();
}

void MVD_InitGame (void)
{
	//	 items
	//game.num_items = sizeof(itemlist)/sizeof(itemlist[0]) - 1;	

	// initialize all entities for this game
	//game.maxentities = maxentities->value;
	//g_edicts = gi.TagMalloc (game.maxentities * sizeof(g_edicts[0]), TAG_GAME);
	//globals.edicts = g_edicts;
	//globals.max_edicts = game.maxentities;
	dllglobals->Init();
	copyDllInfo();
}

void MVD_RunFrame (void)
{
	dllglobals->RunFrame();
	copyDllInfo();
}

void MVD_ServerCommand (void)
{
	dllglobals->ServerCommand();
	copyDllInfo();
}

void copyDllInfo(void)
{
        globals.apiversion = dllglobals->apiversion;
        globals.edict_size = dllglobals->edict_size;
        globals.edicts = dllglobals->edicts;
        globals.num_edicts = dllglobals->num_edicts;
        globals.max_edicts = dllglobals->max_edicts;
}
