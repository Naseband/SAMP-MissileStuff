#if 1
#pragma option -r
#pragma option -d3
#endif

#include <a_samp_extra>
#include <zcmd>
#include <ColAndreas>
#include <FCNPC>
#include <streamer_extra>
#define MISSILE_VEHICLE_CHECK
#include <CAMissiles>
#include <AATurrets>

new TurretID = -1;

new NPCID = -1, VehicleID = -1, NPCState = 0;

public OnFilterScriptInit()
{
	for(new i = 0; i < MAX_PLAYERS; ++i)
	{
		if(FCNPC_IsValid(i))
			FCNPC_Destroy(i);
	}


	TurretID = CreateAATurret(2347.396484, -2109.844726, 37.621528, 0.0, 0.0, 0.0, 0, 0);

	VehicleID = CreateVehicle(511, 0.0, 0.0, 0.0, 0.0, 0, 0, 120);

	NPCID = FCNPC_Create("Pilot4");
	FCNPC_Spawn(NPCID, 0, 0.0, 0.0, 0.0);

	return 1;
}

public OnFilterScriptExit()
{
	//FCNPC_Destroy(NPCID);
	DestroyVehicle(VehicleID);
	return 1;
}

public FCNPC_OnSpawn(npcid)
{
	if(npcid == NPCID)
	{
		SetVehicleToRespawn(VehicleID);
		FCNPC_PutInVehicle(npcid, VehicleID, 0);
		FCNPC_SetPosition(npcid, 2047.396484, -2109.844726, 137.621528);
		FCNPC_GoTo(npcid, 2547.396484, -2009.844726, 107.621528, FCNPC_MOVE_TYPE_AUTO, 2.5);
	}

	return 1;
}

public FCNPC_OnDeath(npcid, killerid, reason)
{
	if(npcid == NPCID)
	{
		SetTimer("respawn", 5000, 0);
		SetVehicleToRespawn(VehicleID);
	}
}

public FCNPC_OnRespawn(npcid)
{
	if(npcid == NPCID)
		FCNPC_OnSpawn(npcid);
}

public respawn()
{
	FCNPC_Respawn(NPCID);
}

public FCNPC_OnReachDestination(npcid)
{
	if(npcid == NPCID)
	{
		if(NPCState == 0)
		{
			FCNPC_GoTo(npcid, 2047.396484, -2009.844726, 107.621528, FCNPC_MOVE_TYPE_AUTO, 2.5);
			NPCState = 1;
		}
		else
		{
			FCNPC_GoTo(npcid, 2547.396484, -2009.844726, 107.621528, FCNPC_MOVE_TYPE_AUTO, 2.5);
			NPCState = 0;
		}
	}

	return 1;
}

CMD:a(playerid, const params[])
{
	PutPlayerInAATurret(playerid, TurretID);

	return 1;
}

CMD:b(playerid, const params[])
{
	RemovePlayerFromAATurret(playerid);

	return 1;
}

public OnAATurretFire(id, playerid, Float:x, Float:y, Float:z, Float:vx, Float:vy, Float:vz)
{
	FireMissile(x, y, z, GetAATurretVirtualWorld(id), GetAATurretInterior(id), vx, vy, vz, MISSILE_MODE_RPG, .ttl = 3000, .speed = 80.0, .jitter = 0.0);
	//FireMissile(x, y, z, GetAATurretVirtualWorld(id), GetAATurretInterior(id), vx, vy, vz, MISSILE_MODE_HOMING, .ttl = 3000, .speed = 80.0, .jitter = 0.0, .target_type = MISSILE_TARGET_VEHICLE, .target_id = VehicleID);

	PlayerPlaySound(playerid, 17005, 0.0, 0.0, 0.0);
}

public OnCAMissileExplode(id, hittype, hitid, bool:swarm, Float:x, Float:y, Float:z, virtual_world, interior)
{
	printf("Hittype %d id %d", hittype, hitid);

	if(hittype == MISSILE_HIT_TYPE_VEHICLE)
	{
		if(hitid == VehicleID && !FCNPC_IsDead(NPCID))
		{
			SetVehicleHealth(hitid, 0.0);
			//FCNPC_RemoveFromVehicle(NPCID);
			//FCNPC_Kill(NPCID);
		}
		else
		{
			SetVehicleHealth(hitid, 0.0);
		}
	}
}
