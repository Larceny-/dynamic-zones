#include <a_samp>

#define FILTERSCRIPT

#include "dynamicgz.inc"

#define DIALOG_ZONE_CREATOR		4571

#define DIALOG_TITLE			"GANGZONE CREATOR v0.2"

#define PlaySelectSound(%0)		PlayerPlaySound(%0,1083,0.0,0.0,0.0)
#define PlayCancelSound(%0)		PlayerPlaySound(%0,1084,0.0,0.0,0.0)
#define PlayErrorSound(%0)		PlayerPlaySound(%0,1085,0.0,0.0,0.0)

new
	bool:g_IsPlayerCreatingZone[MAX_PLAYERS],
	g_PlayerCreatingZoneID[MAX_PLAYERS] = INVALID_GANG_ZONE,
	Float:g_PlayerFistZone[MAX_PLAYERS][2],
	Float:g_GangzonePos[MAX_GANG_ZONES][4],
	g_GangzoneColor[MAX_GANG_ZONES];

stock IsRGB(const string[])
{
    for(new i = 0, j = strlen(string); i < j; i++)
    {
        if((string[i] >= '0' && string[i] <= '9') || (string[i] >= 'a' && string[i] >= 'f') || (string[i] >= 'A' && string[i] >= 'F'))
        	continue;

        return 0;
    }
    return 1;
}

stock HexToInt(string[])
{// WhiteJoker
    if (string[0] == 0)
    {
        return 0;
    }
    new i;
    new cur = 1;
    new res = 0;
    for (i = strlen(string); i > 0; i--)
    {
        if (string[i-1] < 58)
        {
            res = res + cur * (string[i - 1] - 48);
        }
        else
        {
            res = res + cur * (string[i-1] - 65 + 10);
            cur = cur * 16;
        }
    }
    return res;
}

public OnFilterScriptInit()
{
	SendClientMessageToAll(0xA9C4E4FF, "Use /creategz to create a gangzone.");
	printf("GangZone Creator loaded successful.");
	return 1;
}

public OnFilterScriptExit()
{
	printf("GangZone Creator unloaded successful.");
	return 1;
}

public OnPlayerConnect(playerid)
{
	g_IsPlayerCreatingZone[playerid] = false;
	g_PlayerFistZone[playerid][0] = 0.0;
	g_PlayerFistZone[playerid][1] = 0.0;
	return 1;
}

public OnPlayerSpawn(playerid)
{
	SendClientMessageToAll(0xA9C4E4FF, "Use /creategz to create a gangzone.");
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
    if(!strcmp(cmdtext, "/creategz", true))
    {
    	PlaySelectSound(playerid);
        
        //if(g_IsPlayerCreatingZone[playerid] == false) ShowPlayerDialog(playerid, DIALOG_ZONE_CREATOR, DIALOG_STYLE_LIST, DIALOG_TITLE, "1.\tCreate", "Select", "Cancel");
        if(g_IsPlayerCreatingZone[playerid] == false) ShowPlayerDialog(playerid, DIALOG_ZONE_CREATOR+1, DIALOG_STYLE_LIST, DIALOG_TITLE, "1.\tGangZone Position", "Select", "Back");
        else ShowPlayerDialog(playerid, DIALOG_ZONE_CREATOR+1, DIALOG_STYLE_LIST, DIALOG_TITLE, "1.\tGangZone Position\n2.\tGangZone Color\n3.\tExport", "Select", "Back");
        return 1;
    }
    return 0;
}

public OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
	switch(dialogid)
	{
		case DIALOG_ZONE_CREATOR:
		{
			if(!response)
			{
				PlayCancelSound(playerid);
				return 1;
			}

			PlaySelectSound(playerid);
			ShowPlayerDialog(playerid, DIALOG_ZONE_CREATOR+1, DIALOG_STYLE_LIST, DIALOG_TITLE, "1.\tGangZone Position\n2.\tGangZone Color\n3.\tExport", "Select", "Back");
		}
		case DIALOG_ZONE_CREATOR+1:
		{
			if(!response)
			{
				PlayCancelSound(playerid);
				ShowPlayerDialog(playerid, DIALOG_ZONE_CREATOR, DIALOG_STYLE_LIST, DIALOG_TITLE, "1.\tCreate", "Select", "Cancel");
				return 1;
			}

			switch(listitem)
			{
				case 0:
				{
					PlaySelectSound(playerid);
					SendClientMessageToAll(0xA9C4E4FF, "Press ESC go to MAP and mark 2 positions to be the location of the gang zone.");
					g_IsPlayerCreatingZone[playerid] = true;
					g_PlayerCreatingZoneID[playerid] = INVALID_GANG_ZONE;
				}
				case 1:
				{
					PlaySelectSound(playerid);
					ShowPlayerDialog(playerid, DIALOG_ZONE_CREATOR+2, DIALOG_STYLE_INPUT, DIALOG_TITLE, "Input the RGBA color", "Select", "Back");
				}
				case 2:
				{
					new zoneid = g_PlayerCreatingZoneID[playerid];
					if(g_GangzonePos[zoneid][2] == 0.0)
					{
						PlayErrorSound(playerid);
						SendClientMessage(playerid, 0xB9C9BFFF, "You didn't created a gang zone yet.");
						return 1;
					}

					SendClientMessage(playerid, 0xA9C4E4FF, "Exported to created_gangzones.txt");
					SendClientMessage(playerid, 0xFFFFFFFF, "Now you can create a new gang zone.");
					
					new string[128];
					new File:pos = fopen("created_gangzones.txt", io_append);
        			format(string, 128, "CreateDynamicZone(%f, %f, %f, %f, %i);\n", g_GangzonePos[zoneid][0], g_GangzonePos[zoneid][1], g_GangzonePos[zoneid][2], g_GangzonePos[zoneid][3], g_GangzoneColor[zoneid]);
        			fwrite(pos, string);
        			fclose(pos);

        			DestroyDynamicZone(g_PlayerCreatingZoneID[playerid]);

        			g_PlayerCreatingZoneID[playerid] = INVALID_GANG_ZONE;
        			g_IsPlayerCreatingZone[playerid] = false;
        			g_PlayerFistZone[playerid][0] = 0.0;
        			g_PlayerFistZone[playerid][1] = 0.0;
				}
			}
		}
		case DIALOG_ZONE_CREATOR+2:
		{
			if(!response)
			{
				PlayCancelSound(playerid);
				ShowPlayerDialog(playerid, DIALOG_ZONE_CREATOR+1, DIALOG_STYLE_LIST, DIALOG_TITLE, "1.\tGangZone Position\n2.\tGangZone Color\n3.\tExport", "Select", "Back");
				return 1;
			}

			if(!IsRGB(inputtext))
			{
				PlayErrorSound(playerid);
				SendClientMessage(playerid, 0xB9C9BFFF, "Input ONLY HEX colors.");
				ShowPlayerDialog(playerid, DIALOG_ZONE_CREATOR+2, DIALOG_STYLE_INPUT, DIALOG_TITLE, "Input the RGBA color", "Select", "Back");
				return 1;
			}
			else if(strlen(inputtext) != 8)
			{
				PlayErrorSound(playerid);
				SendClientMessage(playerid, 0xB9C9BFFF, "Wrong color format, use ONLY RGBA format.");
				ShowPlayerDialog(playerid, DIALOG_ZONE_CREATOR+2, DIALOG_STYLE_INPUT, DIALOG_TITLE, "Input the RGBA color", "Select", "Back");
				return 1;
			}

			new zoneid = g_PlayerCreatingZoneID[playerid];
			g_GangzoneColor[zoneid] = HexToInt(inputtext);

			SetDynamicZoneColor(zoneid, g_GangzoneColor[zoneid]);
			SendClientMessageToAll(0xA9C4E4FF, "Color defined!");

			PlaySelectSound(playerid);
			ShowPlayerDialog(playerid, DIALOG_ZONE_CREATOR+1, DIALOG_STYLE_LIST, DIALOG_TITLE, "1.\tGangZone Position\n2.\tGangZone Color\n3.\tExport", "Select", "Back");
		}
	}
	return 0;
}

public OnPlayerClickMap(playerid, Float:fX, Float:fY, Float:fZ)
{
    if(g_IsPlayerCreatingZone[playerid])
    {
    	if(g_PlayerFistZone[playerid][0] == 0.0)
    	{
    		g_PlayerFistZone[playerid][0] = fX;
    		g_PlayerFistZone[playerid][1] = fY;
    	}
    	else
    	{
    		g_PlayerCreatingZoneID[playerid] = CreateDynamicZone(g_PlayerFistZone[playerid][0], g_PlayerFistZone[playerid][1], fX, fY, 0xA9C4E4DD);

    		new zoneid = g_PlayerCreatingZoneID[playerid];

    		g_GangzonePos[zoneid][0] = g_PlayerFistZone[playerid][0];
    		g_GangzonePos[zoneid][1] = g_PlayerFistZone[playerid][1];
    		g_GangzonePos[zoneid][2] = fX;
    		g_GangzonePos[zoneid][3] = fY;

    		g_GangzoneColor[zoneid] = 0xA9C4E4DD;

    		ShowDynamicZoneForAll(zoneid);

    		SendClientMessageToAll(0xA9C4E4FF, "Zone created.");
    		PlaySelectSound(playerid);

    		ShowPlayerDialog(playerid, DIALOG_ZONE_CREATOR+1, DIALOG_STYLE_LIST, DIALOG_TITLE, "1.\tGangZone Position\n2.\tGangZone Color\n3.\tExport", "Select", "Back");
    	}
    }
    return 1;
}
