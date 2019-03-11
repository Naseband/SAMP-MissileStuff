#define FILTERSCRIPT

/*

Creates SAM Turrets at A51 and Easter Basin. Will shoot any players on sight!

*/

#include <a_samp>
#define _FOREACH_NO_TEST
#include <foreach>
#include <ColAndreas>
#include <streamer>
#include <CAMissiles>
#include <CASAMTurrets>
#include <zcmd>
#include <sscanf2>

new Float:gArea51PolyCoords[] =
{
	408.2250,2173.6580,	
	459.7497,2165.5764,	
	499.9684,2128.8240,	
	501.6797,2090.5232,	
	501.5017,1994.6135,	
	502.3123,1937.2142,	
	481.9638,1848.4741,	
	457.9689,1799.3912,	
	422.0591,1732.9706,	
	390.1505,1687.9851,	
	346.6827,1643.3701,	
	288.8620,1608.0856,	
	-11.9790,1608.2992,	
	-51.0272,1620.7651,	
	-93.4672,1650.9503,	
	-122.9496,1689.5978,
	-122.5253,1801.5509,
	-142.1570,1873.7998,
	-178.6642,1985.0979,
	-181.9358,2078.4570,
	-143.1168,2146.7803,
	-83.1138,2178.2024,	
	-7.1858,2184.2551,	
	88.0635,2180.9412,	
	168.6694,2181.6987,	
	339.2260,2180.8992,
	408.2250,2173.6580 // first
};

// -----------------------------------------------------------------------------------------

public OnFilterScriptInit()
{
	CA_Init();

	for(new i = 0; i < MAX_PLAYERS; i ++) if(IsPlayerConnected(i))
	{
		OnPlayerConnect(i);
	}

	new id, areaid;

	// Area 51

	areaid = CreateDynamicPolygon(gArea51PolyCoords, 30.0, 600.0, sizeof(gArea51PolyCoords), 0, 0, -1);

	id = CreateSAMTurret(SAM_TYPE_AIR, 18848, 1000, 354.426971, 2028.495239, 22.415220, 0.0, 0.0, 315.0, 40.0, 500.0, 250.0, 65.0, 0.16);
	ToggleSAMTurret(id, 1);
	SetSAMTurretCustomArea(id, areaid);
	
	id = CreateSAMTurret(SAM_TYPE_AIR, 18848, 1000, 237.697906, 1696.875732, 22.415220, 0.0, 0.0, 202.0, 40.0, 500.0, 250.0, 70.0, 0.11);
	ToggleSAMTurret(id, 1);
	SetSAMTurretCustomArea(id, areaid);
	
	id = CreateSAMTurret(SAM_TYPE_AIR, 18848, 1000, 15.614777, 1719.162597, 22.415220, 0.0, 0.0, 135.0, 40.0, 500.0, 250.0, 80.0, 0.09);
	ToggleSAMTurret(id, 1);
	SetSAMTurretCustomArea(id, areaid);
	
	id = CreateSAMTurret(SAM_TYPE_AIR, 18848, 1000, 188.242050, 2081.650878, 22.446470, 0.0, 0.0, 22.5, 40.0, 500.0, 250.0, 90.0, 0.08);
	ToggleSAMTurret(id, 1);
	SetSAMTurretCustomArea(id, areaid);

	// Easter Basin

	id = CreateSAMTurret(SAM_TYPE_AIR, 18848, 300, -1394.8, 493.40, 18.0, 0.0, 0.0, 90.0, 40.0, 500.0, 230.0, 75.0, 0.12, false);
	ToggleSAMTurret(id, 1);

	id = CreateSAMTurret(SAM_TYPE_AIR, 18848, 1000, -1324.33, 493.81, 21.0, 0.0, 0.0, 270.0, 40.0, 500.0, 230.0, 75.0, 0.12, false);
	ToggleSAMTurret(id, 1);


	return 1;
}

public OnFilterScriptExit()
{	
	return 1;
}

// -----------------------------------------------------------------------------------------

public OnPlayerConnect(playerid)
{
	if(IsPlayerNPC(playerid))
	{
		Streamer_ToggleItemUpdate(playerid, STREAMER_TYPE_AREA, 1); // This will allow SAM Turrets to track NPCs in Vehicles, too.
	}
	else
	{
		RemoveBuildingForPlayer(playerid, 3267, 0.0, 0.0, 0.0, 10000.0); // Remove all SAM Turrets (Area 51 type)
		RemoveBuildingForPlayer(playerid, 3884, 0.0, 0.0, 0.0, 10000.0); // Remove all SAM Turrets (Easter Basin type)
	}
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	return 0;
}

// ----------------------------------------------------------------------------------------- Test CMDs

CMD:rotsam(playerid, const params[]) // /rotsam [samid] - Rotates specified SAM towards you
{
	new id;
	if(!sscanf(params, "i", id))
	{
		new Float:x, Float:y, Float:z;
		GetPlayerPos(playerid, x, y, z);

		if(IsValidSAMTurret(id)) RotateSAMTurretTo(id, x, y, z);
	}

	return 1;
}

CMD:firesam(playerid, const params[]) // /firesam [samid] [vid] - Fires specified SAM at specified vehicle ID
{
	new id, vid;
	if(!sscanf(params, "ii", id, vid))
	{
		if(IsValidSAMTurret(id) && GetVehicleModel(vid)) FireSAMTurret(id, vid);
	}

	return 1;
}

CMD:togsam(playerid, const params[]) // /togsam [samid] [toggle(1/0)] - Toggles specified SAM
{
	new id, toggle;
	if(!sscanf(params, "ii", id, toggle))
	{
		if(IsValidSAMTurret(id)) ToggleSAMTurret(id, toggle);
	}

	return 1;
}

CMD:vgod(playerid, const params[]) // /vgod - Gives your vehicle godmode
{
	SetVehicleHealth(GetPlayerVehicleID(playerid), 10000000.0);

	return 1;
}

// -----------------------------------------------------------------------------------------

// EOF
