/* *************************************************************************** *
*	Description:														
*				A SA:MP library containing functions to extend and simplify the management of gang zones.
*														
*	Author:												
*				Larceny									
* 														
*	Contributors:										
*				Y_Less - ALS hooking method				
*				SA-MP Team - (c) SA-MP					
*				ipsBruno and rjj - swap idea			
*	Version:											
*				1.0											
*	Legal:													
*				The MIT License (MIT)
*				Copyright (c) 2014 Larceny
*
*				Permission is hereby granted, free of charge, to any person obtaining a copy
*				of this software and associated documentation files (the "Software"), to deal
*				in the Software without restriction, including without limitation the rights
*				to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*				copies of the Software, and to permit persons to whom the Software is
*				furnished to do so, subject to the following conditions:
*
*				The above copyright notice and this permission notice shall be included in all
*				copies or substantial portions of the Software.
*
*				THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*				IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*				FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*				AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*				LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*				OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*				SOFTWARE.
*
* **************************************************************************** */

#include <a_samp>

// Include Guard
#if defined _dzones_included
	#endinput
#endif
#define _dzones_included

#if !defined MAX_GANG_ZONES
	#define MAX_GANG_ZONES		1024
#endif

// Functions list
/*
native GetDynamicZoneArea(zoneid, &Float:minx, &Float:miny, &Float:maxx, &Float:maxy);
native SetDynamicZoneArea(zoneid, Float:minx, Float:miny, Float:maxx, Float:maxy);
native CreateDynamicZone(Float:minx, Float:miny, Float:maxx, Float:maxy, color);
native FlashDynamicZoneForPlayer(playerid, zoneid, color);
native IsDynamicZoneFlashingForPlayer(playerid, zoneid);
native IsDynamicZoneVisibleForPlayer(playerid, zoneid);
native StopFlashDynamicZoneForPlayer(playerid, zoneid);
native ShowDynamicZoneForPlayer(playerid, zoneid);
native HideDynamicZoneForPlayer(playerid, zoneid);
native IsPlayerInDynamicZone(playerid, zoneid);
native FlashDynamicZoneForAll(zoneid, color);
native SetDynamicZoneColor(zoneid, color);
native StopFlashDynamicZoneForAll(zoneid);
native ShowDynamicZoneForAll(zoneid);
native HideDynamicZoneForAll(zoneid);
native GetDynamicZoneColor(zoneid);
native DestroyDynamicZone(zoneid);
*/

//------------------------------------------------------------------------------

enum E_GANG_ZONE
{
	E_GANG_ZONE_ID,
	E_GANG_ZONE_COLOR,
	Float:E_GANG_ZONE_MINX,
	Float:E_GANG_ZONE_MINY,
	Float:E_GANG_ZONE_MAXX,
	Float:E_GANG_ZONE_MAXY
}
static stock
	g_DynamicZone[MAX_GANG_ZONES][E_GANG_ZONE],
	//g_TotalGangzones, // Total of created gang zones
	g_TimerIndex;

//------------------------------------------------------------------------------

forward DynamicZonesTimer();

//------------------------------------------------------------------------------

// (c) by Y_Less
#define swap(%1,%2) \
	%1 ^= %2, %2 ^= %1, %1 ^= %2

stock CreateDynamicZone(Float:minx, Float:miny, Float:maxx, Float:maxy, color = 0xFFFFFFFF)
{
	new zoneid = GetZoneFreeID();

	if(zoneid == INVALID_GANG_ZONE)
	{
		print("ERROR: Limit of zones exceeded.");
		return 0;
	}

	if(minx > maxx)
		swap(minx, maxx);

	if(miny > maxy)
		swap(miny, maxy);

	g_DynamicZone[zoneid][E_GANG_ZONE_ID]		= GangZoneCreate(minx, miny, maxx, maxy);
	g_DynamicZone[zoneid][E_GANG_ZONE_COLOR]	= color;
	g_DynamicZone[zoneid][E_GANG_ZONE_MINX]		= minx;
	g_DynamicZone[zoneid][E_GANG_ZONE_MINY]		= miny;
	g_DynamicZone[zoneid][E_GANG_ZONE_MAXX]		= maxx;
	g_DynamicZone[zoneid][E_GANG_ZONE_MAXY]		= maxy;

	//g_TotalGangzones++;
	return zoneid;
}

stock DestroyDynamicZone(zoneid)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

    GangZoneDestroy(g_DynamicZone[zoneid][E_GANG_ZONE_ID]);
	//g_TotalGangzones--;
    return 1;
}

stock SetDynamicZoneArea(zoneid, Float:minx, Float:miny, Float:maxx, Float:maxy)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

    GangZoneDestroy(g_DynamicZone[zoneid][E_GANG_ZONE_ID]);

	g_DynamicZone[zoneid][E_GANG_ZONE_MINX] = minx;
	g_DynamicZone[zoneid][E_GANG_ZONE_MINY] = miny;
	g_DynamicZone[zoneid][E_GANG_ZONE_MAXX] = maxx;
	g_DynamicZone[zoneid][E_GANG_ZONE_MAXY] = maxy;
	g_DynamicZone[zoneid][E_GANG_ZONE_ID]	= GangZoneCreate(minx, miny, maxx, maxy);

	new PVarString[12];
	format(PVarString, sizeof(PVarString), "vszone_%i", zoneid);
	for(new slots = GetMaxPlayers(), i; i < slots; i++)
	{
	    if(!IsPlayerConnected(i))
	        continue;
	   
		if(GetPVarInt(playerid, PVarString))
		{
			GangZoneHideForPlayer(i, g_DynamicZone[zoneid][E_GANG_ZONE_ID]);
			GangZoneShowForPlayer(i, g_DynamicZone[zoneid][E_GANG_ZONE_ID], g_DynamicZone[zoneid][E_GANG_ZONE_COLOR);
		}
	}	
	return 1;
}

stock GetDynamicZoneArea(zoneid, &Float:minx, &Float:miny, &Float:maxx, &Float:maxy)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

	minx = g_DynamicZone[zoneid][E_GANG_ZONE_MINX];
	miny = g_DynamicZone[zoneid][E_GANG_ZONE_MINY];
	maxx = g_DynamicZone[zoneid][E_GANG_ZONE_MAXX];
	maxy = g_DynamicZone[zoneid][E_GANG_ZONE_MAXY];
	return 1;
}

stock ShowDynamicZoneForPlayer(playerid, zoneid)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

	if(!IsPlayerConnected(playerid))
		return 0;

    GangZoneShowForPlayer(playerid, g_DynamicZone[zoneid][E_GANG_ZONE_ID], g_DynamicZone[zoneid][E_GANG_ZONE_COLOR]);

	new PVarString[12];
	format(PVarString, sizeof(PVarString), "vszone_%i", zoneid);
	SetPVarInt(playerid, PVarString, 1);
	return 1;
}

stock HideDynamicZoneForPlayer(playerid, zoneid)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

	if(!IsPlayerConnected(playerid))
		return -1;

    GangZoneHideForPlayer(playerid, g_DynamicZone[zoneid][E_GANG_ZONE_ID]);

	new PVarString[12];
	format(PVarString, sizeof(PVarString), "vszone_%i", zoneid);
	DeletePVar(playerid, PVarString);
	return 1;
}

stock ShowDynamicZoneForAll(zoneid)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

	new PVarString[12];
	format(PVarString, sizeof(PVarString), "vszone_%i", zoneid);
    for(new slots = GetMaxPlayers(), i; i < slots; i++)
	{
	    if(!IsPlayerConnected(i))
	        continue;

		GangZoneShowForPlayer(i, g_DynamicZone[zoneid][E_GANG_ZONE_ID], g_DynamicZone[zoneid][E_GANG_ZONE_COLOR]);
		SetPVarInt(i, PVarString, 1);
	}
	return 1;
}

stock HideDynamicZoneForAll(zoneid)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

	new PVarString[12];
	format(PVarString, sizeof(PVarString), "vszone_%i", zoneid);
    for(new slots = GetMaxPlayers(), i; i < slots; i++)
	{
	    if(!IsPlayerConnected(i))
	        continue;

		GangZoneHideForPlayer(i, g_DynamicZone[zoneid][E_GANG_ZONE_ID]);
		DeletePVar(i, PVarString);
	}
	return 1;
}

stock FlashDynamicZoneForAll(zoneid, color)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

	new PVarString[12];
	new PVarFlashing[12];
	format(PVarString, sizeof(PVarString), "vszone_%i", zoneid);
	format(PVarFlashing, sizeof(PVarFlashing), "fszone_%i", zoneid);
    for(new slots = GetMaxPlayers(), i; i < slots; i++)
	{
	    if(!IsPlayerConnected(i))
	        continue;

		if(GetPVarInt(i, PVarString))
		{
			GangZoneFlashForPlayer(i, g_DynamicZone[zoneid][E_GANG_ZONE_ID], color);
			SetPVarInt(i, PVarFlashing, 1);
		}
	}
	return 1;
}

stock StopFlashDynamicZoneForAll(zoneid)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

	new PVarString[12];
	new PVarFlashing[12];
	format(PVarString, sizeof(PVarString), "vszone_%i", zoneid);
	format(PVarFlashing, sizeof(PVarFlashing), "fszone_%i", zoneid);
    for(new slots = GetMaxPlayers(), i; i < slots; i++)
	{
	    if(!IsPlayerConnected(i))
	        continue;

		if(GetPVarInt(i, PVarString))
		{
			GangZoneStopFlashForPlayer(i, g_DynamicZone[zoneid][E_GANG_ZONE_ID]);
			DeletePVar(i, PVarFlashing);
		}
	}
	return 1;
}

stock FlashDynamicZoneForPlayer(playerid, zoneid, color)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

	if(!IsPlayerConnected(playerid))
		return 0;

	new PVarString[12];
	new PVarFlashing[12];
	format(PVarString, sizeof(PVarString), "vszone_%i", zoneid);
	format(PVarFlashing, sizeof(PVarFlashing), "fszone_%i", zoneid);
	if(GetPVarInt(playerid, PVarString))
	{
		GangZoneFlashForPlayer(playerid, g_DynamicZone[zoneid][E_GANG_ZONE_ID], color);
		SetPVarInt(playerid, PVarFlashing, 1);
	}
	return 1;
}

stock StopFlashDynamicZoneForPlayer(playerid, zoneid)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

	if(!IsPlayerConnected(playerid))
		return 0;

	new PVarString[12];
	new PVarFlashing[12];
	format(PVarString, sizeof(PVarString), "vszone_%i", zoneid);
	format(PVarFlashing, sizeof(PVarFlashing), "fszone_%i", zoneid);
	if(GetPVarInt(playerid, PVarString))
	{
		GangZoneStopFlashForPlayer(playerid, g_DynamicZone[zoneid][E_GANG_ZONE_ID]);
		DeletePVar(playerid, PVarFlashing);
	}
	return 1;
}

stock SetDynamicZoneColor(zoneid, color)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

	g_DynamicZone[zoneid][E_GANG_ZONE_COLOR] = color;

	new PVarString[12];
	format(PVarString, sizeof(PVarString), "vszone_%i", zoneid);
    for(new slots = GetMaxPlayers(), i; i < slots; i++)
	{
	    if(!IsPlayerConnected(i))
	        continue;

		if(GetPVarInt(i, PVarString))
		{
			GangZoneHideForPlayer(i, g_DynamicZone[zoneid][E_GANG_ZONE_ID]);
			GangZoneShowForPlayer(i, g_DynamicZone[zoneid][E_GANG_ZONE_ID], g_DynamicZone[zoneid][E_GANG_ZONE_COLOR]);
		}
	}
	return 1;
}

stock GetDynamicZoneColor(zoneid)
{
	if(g_DynamicZone[zoneid][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
		return 0;

	return g_DynamicZone[zoneid][E_GANG_ZONE_COLOR];
}

stock IsDynamicZoneVisibleForPlayer(playerid, zoneid)
{
	new PVarString[12];
	format(PVarString, sizeof(PVarString), "vszone_%i", zoneid);
	return (GetPVarInt(playerid, PVarString)) ? 1 : 0;
}

stock IsDynamicZoneFlashingForPlayer(playerid, zoneid)
{
	new PVarString[12];
	format(PVarString, sizeof(PVarString), "fszone_%i", zoneid);
	return (GetPVarInt(playerid, PVarString)) ? 1 : 0;
}

stock IsPlayerInDynamicZone(playerid, zoneid)
{
    new	Float:x, Float:y, Float:z;
    GetPlayerPos(playerid, x, y, z);

    if(x > g_DynamicZone[zoneid][E_GANG_ZONE_MINX] && x < g_DynamicZone[zoneid][E_GANG_ZONE_MAXX] && y > g_DynamicZone[zoneid][E_GANG_ZONE_MINY] && y < g_DynamicZone[zoneid][E_GANG_ZONE_MAXY])
    	return 1;
    return 0;
}

stock GetZoneFreeID()
{
	for(new z = 0; z < MAX_GANG_ZONES; z++)
	{
		if(g_DynamicZone[z][E_GANG_ZONE_ID] == INVALID_GANG_ZONE)
			return z;
	}
	return INVALID_GANG_ZONE;
}

public DynamicZonesTimer()
{
	new PVarString[12];
    for(new slots = GetMaxPlayers(), i; i < slots; i++)
	{
	    if(!IsPlayerConnected(i))
	        continue;

		//for(new z = 0; z < g_TotalGangzones; z++)
		for(new z = 0; z < MAX_GANG_ZONES; z++)
		{
			format(PVarString, sizeof(PVarString), "etzone_%i", z);
			if(IsPlayerInDynamicZone(i, z) && !GetPVarInt(i, PVarString))
			{
				CallLocalFunction("OnPlayerEnterDynamicZone", "ii",	i, z);
				SetPVarInt(i, PVarString, 1);
			}
			else if(!IsPlayerInDynamicZone(i, z) && GetPVarInt(i, PVarString))
			{
				CallLocalFunction("OnPlayerLeaveDynamicZone", "ii", i, z);
				DeletePVar(i, PVarString);
			}
		}
	}
	return 1;
}

#if defined FILTERSCRIPT
	public OnFilterScriptInit()
	{
		for(new z = 0; z < MAX_GANG_ZONES; z++)
		{
			g_DynamicZone[z][E_GANG_ZONE_ID] = INVALID_GANG_ZONE;
		}
		g_TimerIndex = SetTimer("DynamicZonesTimer", 1000, true);
		#if defined dyZones_OnFilterScriptInit
			return dyZones_OnFilterScriptInit();
		#else
			return 1;
		#endif
	}
	#if defined _ALS_OnFilterScriptInit
		#undef OnFilterScriptInit
	#else
		#define _ALS_OnFilterScriptInit
	#endif
	 
	#define OnFilterScriptInit dyZones_OnFilterScriptInit
	#if defined dyZones_OnFilterScriptInit
		forward dyZones_OnFilterScriptInit();
	#endif

	public OnFilterScriptExit()
	{
		KillTimer(g_TimerIndex);
		#if defined dyZones_OnFilterScriptExit
			return dyZones_OnFilterScriptExit();
		#else
			return 1;
		#endif
	}
	#if defined _ALS_OnFilterScriptExit
		#undef OnFilterScriptExit
	#else
		#define _ALS_OnFilterScriptExit
	#endif
	 
	#define OnFilterScriptExit dyZones_OnFilterScriptExit
	#if defined dyZones_OnFilterScriptExit
		forward dyZones_OnFilterScriptExit();
	#endif
#else
	public OnGameModeInit()
	{
		for(new z = 0; z < MAX_GANG_ZONES; z++)
		{
			g_DynamicZone[z][E_GANG_ZONE_ID] = INVALID_GANG_ZONE;
		}
		g_TimerIndex = SetTimer("DynamicZonesTimer", 1000, true);
		#if defined dyZones_OnGameModeInit
			return dyZones_OnGameModeInit();
		#else
			return 1;
		#endif
	}
	#if defined _ALS_OnGameModeInit
		#undef OnGameModeInit
	#else
		#define _ALS_OnGameModeInit
	#endif
	 
	#define OnGameModeInit dyZones_OnGameModeInit
	#if defined dyZones_OnGameModeInit
		forward dyZones_OnGameModeInit();
	#endif

	public OnGameModeExit()
	{
		KillTimer(g_TimerIndex);
		#if defined dyZones_OnGameModeExit
			return dyZones_OnGameModeExit();
		#else
			return 1;
		#endif
	}
	#if defined _ALS_OnGameModeExit
		#undef OnGameModeExit
	#else
		#define _ALS_OnGameModeExit
	#endif
	 
	#define OnGameModeExit dyZones_OnGameModeExit
	#if defined dyZones_OnGameModeExit
		forward dyZones_OnGameModeExit();
	#endif
#endif
#undef swap

forward OnPlayerEnterDynamicZone(playerid, zoneid);
forward OnPlayerLeaveDynamicZone(playerid, zoneid);
