#if defined __SAM_TURRETS_INCLUDED_
#endinput
#endif
#define __SAM_TURRETS_INCLUDED_

// ----------------------------------------------------------------------------------------- Config/Limits

#if !defined MAX_SAM_TURRETS
#define MAX_SAM_TURRETS				20
#endif

#if !defined MAX_SAM_TURRET_HEALTH
#define MAX_SAM_TURRET_HEALTH		500.0
#endif

#if !defined SAM_TURRET_ROT_SPEED
#define SAM_TURRET_ROT_SPEED 		100.0
#endif

#if !defined SAM_TURRET_EXTRA_ID
#define SAM_TURRET_EXTRA_ID			15000000 // Streamer Item Extra Array ID (slot 0)
#endif

#if !defined SAM_TURRET_WARNING_SOUND
#define SAM_TURRET_WARNING_SOUND	5201 // Plays this sound if a SAM Turret fires a missile, set to 0 to disable
#endif

// ----------------------------------------------------------------------------------------- Main Data

enum (+=1)
{
	SAM_TYPE_AIR,
	SAM_TYPE_GROUND
};

enum (+=1)
{
	SAM_STATE_OFF,
	SAM_STATE_IDLE,
	SAM_STATE_TRACKING,
	SAM_STATE_RESET
};

enum E_SAM_TURRET
{
	bool:samExists,

	samType,
	samModelID,
	Float:samRange,
	Float:samSpeed,
	Float:samSkill,
	bool:samCanRotate,
	Float:samRotSpeed,
	samVirtualWorld,
	samInterior,

	samState,
	samTargetID,
	samLastAttack,
	samInterval,
	Float:samHealth,

	Float:samX,
	Float:samY,
	Float:samZ,
	Float:samRX,
	Float:samRY,
	Float:samRZ,

	Float:samOrgRZ,

	samObjectID,
	samAreaID,
	samLabelID,
	samTimerID
};
new static 	gSAMTurrets[MAX_SAM_TURRETS][E_SAM_TURRET],
			gSAMTurretHostileState[MAX_SAM_TURRETS][MAX_PLAYERS char];

new static gPlayerLastSoundTS[MAX_PLAYERS];

new static bool:gSAMTurretsInit = false;

// ----------------------------------------------------------------------------------------- Vehicle Types

#define VEHICLE_NONE         0
#define VEHICLE_CAR          1
#define VEHICLE_BOAT         2
#define VEHICLE_TRAIN        3
#define VEHICLE_HELI         4
#define VEHICLE_PLANE        5
#define VEHICLE_BIKE         6
#define VEHICLE_MONSTERTRUCK 7
#define VEHICLE_BMX          8
#define VEHICLE_TRAILER      9
#define VEHICLE_TANK         10

new static const gs_vehicleTypes [212] =
{
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_MONSTERTRUCK,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_BOAT,
    VEHICLE_CAR, VEHICLE_TANK, VEHICLE_TANK, VEHICLE_CAR, VEHICLE_TRAILER, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_MONSTERTRUCK, VEHICLE_CAR,
    VEHICLE_BOAT, VEHICLE_HELI, VEHICLE_BIKE, VEHICLE_TRAIN, VEHICLE_TRAILER, VEHICLE_CAR, VEHICLE_BOAT,
    VEHICLE_BOAT, VEHICLE_BOAT, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_PLANE,
    VEHICLE_BIKE, VEHICLE_BIKE, VEHICLE_BIKE, VEHICLE_PLANE, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_BIKE, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_BIKE, VEHICLE_BOAT, VEHICLE_BOAT, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_PLANE, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_BMX, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_BOAT, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_BOAT, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_BMX, VEHICLE_BMX, VEHICLE_PLANE, VEHICLE_PLANE, VEHICLE_PLANE,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_PLANE, VEHICLE_PLANE, VEHICLE_BIKE,
    VEHICLE_BIKE, VEHICLE_BIKE, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_TRAIN,
    VEHICLE_TRAIN, VEHICLE_PLANE, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_PLANE,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_MONSTERTRUCK, VEHICLE_MONSTERTRUCK, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_HELI, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_TRAIN, VEHICLE_TRAIN, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_MONSTERTRUCK, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_PLANE, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_BIKE, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_TRAILER, VEHICLE_CAR, VEHICLE_BIKE, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_TRAIN,
    VEHICLE_TRAILER, VEHICLE_PLANE, VEHICLE_PLANE, VEHICLE_CAR, VEHICLE_BOAT, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR, VEHICLE_CAR,
    VEHICLE_TRAILER, VEHICLE_TRAILER, VEHICLE_TRAILER, VEHICLE_CAR, VEHICLE_TRAILER, VEHICLE_TRAILER
};

// ----------------------------------------------------------------------------------------- Forwarding

forward Float:RetRZFromVectorXY(Float:vx, Float:vy);
forward Float:floatangledistdir(Float:firstAngle, Float:secondAngle);
forward SAMTimer(id);

// ----------------------------------------------------------------------------------------- Hooks

public OnFilterScriptInit()
{
	SAM_Init();


	#if defined SAM_OnFilterScriptInit
		return SAM_OnFilterScriptInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif

#define OnFilterScriptInit SAM_OnFilterScriptInit
#if defined SAM_OnFilterScriptInit
	forward SAM_OnFilterScriptInit();
#endif


public OnFilterScriptExit()
{
	SAM_Exit();


	#if defined SAM_OnFilterScriptExit
		return SAM_OnFilterScriptExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnFilterScriptExit
	#undef OnFilterScriptExit
#else
	#define _ALS_OnFilterScriptExit
#endif

#define OnFilterScriptExit SAM_OnFilterScriptExit
#if defined SAM_OnFilterScriptExit
	forward SAM_OnFilterScriptExit();
#endif


public OnGameModeInit()
{
	SAM_Init();


	#if defined SAM_OnGameModeInit
		return SAM_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit SAM_OnGameModeInit
#if defined SAM_OnGameModeInit
	forward SAM_OnGameModeInit();
#endif


public OnGameModeExit()
{
	SAM_Exit();

	
	#if defined SAM_OnGameModeExit
		return SAM_OnGameModeExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeExit
	#undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif

#define OnGameModeExit SAM_OnGameModeExit
#if defined SAM_OnGameModeExit
	forward SAM_OnGameModeExit();
#endif


public OnPlayerConnect(playerid)
{
	gPlayerLastSoundTS[playerid] = GetTickCount();

	//for(new i = 0; i < MAX_SAM_TURRETS; i ++) SetSAMTurretHostileState(i, playerid, 1);


	#if defined SAM_OnPlayerConnect
		return SAM_OnPlayerConnect(playerid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerConnect
	#undef OnPlayerConnect
#else
	#define _ALS_OnPlayerConnect
#endif

#define OnPlayerConnect SAM_OnPlayerConnect
#if defined SAM_OnPlayerConnect
	forward SAM_OnPlayerConnect(playerid);
#endif

// ----------------------------------------------------------------------------------------- Internal Functions

stock static SAM_Init()
{
	if(gSAMTurretsInit) return 0;

	gSAMTurretsInit = true;

	return 1;
}

stock static SAM_Exit()
{
	if(!gSAMTurretsInit) return 0;

	DestroyAllSAMTurrets();

	gSAMTurretsInit = false;

	return 1;
}

// ----------------------------------------------------------------------------------------- External Functions

stock CreateSAMTurret(type, modelid, interval, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:min_z, Float:max_z, Float:range = 100.0, Float:speed = 30.0, Float:skill = 0.4, bool:can_rotate = true, Float:rot_speed = SAM_TURRET_ROT_SPEED, virtual_world = 0, interior = 0)
{
	if(!gSAMTurretsInit) return -1;

	new id = -1;

	for(new i = 0; i < MAX_SAM_TURRETS; i ++) if(!gSAMTurrets[i][samExists])
	{
		id = i;
		break;
	}

	if(id == -1) return -1;

	gSAMTurrets[id][samExists] = true;

	gSAMTurrets[id][samType] = type;
	gSAMTurrets[id][samModelID] = modelid;
	gSAMTurrets[id][samRange] = range;
	gSAMTurrets[id][samSpeed] = speed;
	gSAMTurrets[id][samSkill] = skill;
	gSAMTurrets[id][samCanRotate] = can_rotate;
	gSAMTurrets[id][samRotSpeed] = rot_speed;
	gSAMTurrets[id][samVirtualWorld] = virtual_world;
	gSAMTurrets[id][samInterior] = interior;

	gSAMTurrets[id][samState] = SAM_STATE_OFF;
	gSAMTurrets[id][samTargetID] = -1;
	gSAMTurrets[id][samLastAttack] = gettime();
	gSAMTurrets[id][samInterval] = interval;

	gSAMTurrets[id][samX] = x;
	gSAMTurrets[id][samY] = y;
	gSAMTurrets[id][samZ] = z;
	gSAMTurrets[id][samRX] = rx;
	gSAMTurrets[id][samRY] = ry;
	gSAMTurrets[id][samRZ] = rz;
	gSAMTurrets[id][samOrgRZ] = rz;

	new extra_data[2];
	extra_data[0] = SAM_TURRET_EXTRA_ID;
	extra_data[1] = id;

	new objectid = CreateDynamicObject(modelid, x, y, z, rx, ry, rz, virtual_world, interior, -1, 400.0, 400.0, .priority = 1);	

	Streamer_SetArrayData(STREAMER_TYPE_OBJECT, objectid, E_STREAMER_EXTRA_ID, extra_data);
	
	new areaid = CreateDynamicCylinder(x, y, min_z, max_z, range, virtual_world, interior, -1, 1);

	Streamer_SetArrayData(STREAMER_TYPE_AREA, areaid, E_STREAMER_EXTRA_ID, extra_data);
	
	new Text3D:labelid = CreateDynamic3DTextLabel("[||||||||||]", 0xFFFFFFFF, x, y, z + 2.8, range, _, _, 0, virtual_world, interior, -1, range);

	Streamer_SetArrayData(STREAMER_TYPE_3D_TEXT_LABEL, labelid, E_STREAMER_EXTRA_ID, extra_data);

	gSAMTurrets[id][samObjectID] = objectid;
	gSAMTurrets[id][samAreaID] = areaid;
	gSAMTurrets[id][samLabelID] = _:labelid;	

	gSAMTurrets[id][samTimerID] = SetTimerEx("SAMTimer", interval, 1, "i", id);

	SetSAMTurretHostileState(id, -1, 1); // Hostile to everyone by default

	return id;
}

stock IsValidSAMTurret(id)
{
	if(id < 0 || id >= MAX_SAM_TURRETS || !gSAMTurrets[id][samExists]) return 0;

	return 1;
}

stock DestroySAMTurret(id)
{
	if(!IsValidSAMTurret(id)) return 0;

	if(IsValidDynamicObject(gSAMTurrets[id][samObjectID])) DestroyDynamicObject(gSAMTurrets[id][samObjectID]);
	gSAMTurrets[id][samObjectID] = -1;

	if(IsValidDynamicArea(gSAMTurrets[id][samAreaID])) DestroyDynamicArea(gSAMTurrets[id][samAreaID]);
	gSAMTurrets[id][samAreaID] = -1;

	if(gSAMTurrets[id][samTimerID] != -1) KillTimer(gSAMTurrets[id][samTimerID]);
	gSAMTurrets[id][samTimerID] = -1;

	gSAMTurrets[id][samExists] = false;

	return 1;
}

stock DestroyAllSAMTurrets()
{
	for(new id = 0; id < MAX_SAM_TURRETS; id ++) DestroySAMTurret(id);

	return 1;
}

stock ToggleSAMTurret(id, toggle)
{
	if(toggle)
	{
		gSAMTurrets[id][samState] = SAM_STATE_IDLE;
	}
	else
	{
		gSAMTurrets[id][samState] = SAM_STATE_OFF;
	}
}

stock GetSAMTurretBarrelPos(modelid, Float:base_x, Float:base_y, Float:base_z, Float:base_rx, Float:base_rz, &Float:x, &Float:y, &Float:z, &Float:vx, &Float:vy, &Float:vz)
{
	switch(modelid)
	{
		case 3884, 18848:
		{
			x = base_x + 1.7 * floatsin(-base_rz, degrees);
			y = base_y + 1.7 * floatsin(-base_rz, degrees);
			z = base_z + 2.4;

			GetVectorFromRot(base_rx + 30.0, base_rz, vx, vy, vz);
		}

		default:
		{
			printf("[SAM Warning] No Barrel position for Model %d", modelid);

			x = base_x;
			y = base_y;
			z = base_z;

			GetVectorFromRot(base_rx, base_rz, vx, vy, vz);
		}
	}
}

stock SetSAMTurretCustomArea(id, areaid) // Changes the area ID to the specified one, allowing for example polygon areas like the A51 example. Note: The max. range still applies
{
	if(!IsValidDynamicArea(areaid)) return 0;
	
	if(IsValidDynamicArea(gSAMTurrets[id][samAreaID])) DestroyDynamicArea(gSAMTurrets[id][samAreaID]);

	gSAMTurrets[id][samAreaID] = areaid;

	return 1;
}

stock Float:RotateSAMTurret(id, Float:rz)
{
	if(!gSAMTurrets[id][samCanRotate]) return 0.0; // This function returns the angle delta to rotate below, so if we just return 0.0 it will fire even if not rotated (for non-rotating turets)

	new Float:orx, Float:ory, Float:orz,
		objectid = gSAMTurrets[id][samObjectID];

	GetDynamicObjectRot(objectid, orx, ory, orz);

	new Float:delta_rz = floatabs(floatangledistdir(orz, rz));

	if(delta_rz < 0.1) return 0.0;

	new Float:speed = 0.04 / delta_rz * gSAMTurrets[id][samRotSpeed]; // deg/s

	SetDynamicObjectPos(objectid, gSAMTurrets[id][samX], gSAMTurrets[id][samY], gSAMTurrets[id][samZ]);
	MoveDynamicObject(objectid, gSAMTurrets[id][samX], gSAMTurrets[id][samY], gSAMTurrets[id][samZ] + 0.04, speed, gSAMTurrets[id][samRX], gSAMTurrets[id][samRY], rz);

	return delta_rz;
}

stock Float:RotateSAMTurretTo(id, Float:x, Float:y, Float:z)
{
	new Float:vx = x - gSAMTurrets[id][samX],
		Float:vy = y - gSAMTurrets[id][samY],
		Float:vz = z - gSAMTurrets[id][samZ];

	NormalizeVector(vx, vy, vz);

	new Float:rz = RetRZFromVectorXY(vx, vy);

	return RotateSAMTurret(id, rz);
}

stock FireSAMTurret(id, targetid)
{
	new Float:x, Float:y, Float:z, Float:vx, Float:vy, Float:vz,
		Float:base_x = gSAMTurrets[id][samX], Float:base_y = gSAMTurrets[id][samY], Float:base_z = gSAMTurrets[id][samZ],
		Float:base_rx, Float:base_ry, Float:base_rz;

	GetDynamicObjectRot(gSAMTurrets[id][samObjectID], base_rx, base_ry, base_rz);

	GetSAMTurretBarrelPos(gSAMTurrets[id][samModelID], base_x, base_y, base_z, base_rx, base_rz, x, y, z, vx, vy, vz);

	#if SAM_TURRET_WARNING_SOUND != 0

		new ts = GetTickCount();

		foreach(new playerid : Player) if(ts - gPlayerLastSoundTS[playerid] >= 1000 && GetPlayerVehicleID(playerid) == targetid)
		{
			PlayerPlaySound(playerid, SAM_TURRET_WARNING_SOUND, 0.0, 0.0, 0.0);

			gPlayerLastSoundTS[playerid] = ts;
		}

	#endif

	return FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_HOMING, 25000, gSAMTurrets[id][samSpeed], 1.0, 0.03, gSAMTurrets[id][samSkill], .target_type = MISSILE_TARGET_VEHICLE, .target_id = targetid);
}

stock FindSAMTurretTarget(id)
{
	if(IsAnyPlayerInDynamicArea(gSAMTurrets[id][samAreaID]))
	{
		new Float:dist, Float:tmpdist, targetid = -1,
			Float:x = gSAMTurrets[id][samX], Float:y = gSAMTurrets[id][samY], Float:z = gSAMTurrets[id][samZ],
			Float:tx, Float:ty, Float:tz,
			Float:range = gSAMTurrets[id][samRange];

		foreach(new playerid : Character) if(IsPlayerInDynamicArea(playerid, gSAMTurrets[id][samAreaID]) && GetSAMTurretHostileState(id, playerid))
		{
			new vid = GetPlayerVehicleID(playerid);

			if(vid)
			{
				new modeltype = GetVehicleModelType(GetVehicleModel(vid)), bool:valid = false;

				switch(gSAMTurrets[id][samType])
				{
					case SAM_TYPE_AIR:
					{
						if(	modeltype == VEHICLE_PLANE ||
							modeltype == VEHICLE_HELI) valid = true;
					}
					case SAM_TYPE_GROUND:
					{
						if(	modeltype == VEHICLE_CAR ||
							modeltype == VEHICLE_BOAT ||
							modeltype == VEHICLE_BIKE || 
							modeltype == VEHICLE_MONSTERTRUCK || 
							modeltype == VEHICLE_TANK) valid = true;
					}
				}

				if(valid)
				{
					GetVehiclePos(vid, tx, ty, tz);

					tx = x - tx;
					ty = y - ty;
					tz = z - tz;

					if(VectorSize(tx, ty, 0.0) < range) // Only shoot if in 2D range
					{
						if(targetid == -1 || (tmpdist = VectorSize(tx, ty, tz)) < dist) // We still want to find the closest target in 3D space
						{
							targetid = vid;
							dist = tmpdist;
						}
					}
				}
			}
		}

		if(targetid != -1)
		{
			gSAMTurrets[id][samState] = SAM_STATE_TRACKING;
			gSAMTurrets[id][samTargetID] = targetid;

			return 1;
		}
	}

	gSAMTurrets[id][samTargetID] = -1;
	gSAMTurrets[id][samState] = SAM_STATE_IDLE;

	return 0;
}

stock SetSAMTurretHostileState(id, playerid, hostile_state)
{
	if(playerid == -1)
	{
		for(new i = 0; i < MAX_PLAYERS; i ++) SetSAMTurretHostileState(id, i, hostile_state);
	}
	else
	{
		if(!IsValidSAMTurret(id)) return 0;

		gSAMTurretHostileState[id]{playerid} = (hostile_state ? 1 : 0);
	}

	return 1;
}

stock GetSAMTurretHostileState(id, playerid)
{
	return gSAMTurretHostileState[id]{playerid};
}

// ----------------------------------------------------------------------------------------- Public Functions (eg Timers)

public SAMTimer(id)
{
	if(!IsValidSAMTurret(id)) return 1;

	new ts = gettime();

	switch(gSAMTurrets[id][samState])
	{
		case SAM_STATE_IDLE:
		{
			if(!FindSAMTurretTarget(id))
			{
				if(ts - gSAMTurrets[id][samLastAttack] > 15)
				{
					gSAMTurrets[id][samState] = SAM_STATE_RESET;
					gSAMTurrets[id][samLastAttack] = ts;
				}
			}
		}

		case SAM_STATE_TRACKING:
		{
			if(FindSAMTurretTarget(id))
			{
				new vid = gSAMTurrets[id][samTargetID], Float:x, Float:y, Float:z;

				GetVehiclePos(vid, x, y, z);

				if(IsPointInDynamicArea(gSAMTurrets[id][samAreaID], x, y, z) && IsVehicleOccupied(vid))
				{
					new Float:delta_rz = RotateSAMTurretTo(id, x, y, z);

					if(delta_rz < 35.0)
					{
						FireSAMTurret(id, vid);
						gSAMTurrets[id][samLastAttack] = ts;
					}
				}
				else
				{
					gSAMTurrets[id][samState] = SAM_STATE_IDLE;
				}
			}
			else
			{
				gSAMTurrets[id][samState] = SAM_STATE_IDLE;
			}
		}

		case SAM_STATE_RESET:
		{
			RotateSAMTurret(id, gSAMTurrets[id][samOrgRZ]);

			gSAMTurrets[id][samState] = SAM_STATE_IDLE;
		}
	}

	return 1;
}

// ----------------------------------------------------------------------------------------- Util

stock static Float:RetRZFromVectorXY(Float:vx, Float:vy)
{
	if(vx == 0.0 && vy == 0.0) return 0.0;

	return atan2(vy, vx) - 90.0;
}

stock static Float:RetRXFromVectorZ(Float:vz)
{
	return -(acos(vz) - 90.0);
}

stock static NormalizeVector(&Float:x, &Float:y, &Float:z)
{
	new Float: len = VectorSize(x, y, z);

	if(len == 0.0) return 0;

	x = x / len;
	y = y / len;
	z = z / len;

	return 1;
}

stock static GetVectorFromRot(Float:rx, Float:rz, &Float:vx, &Float:vy, &Float:vz)
{
	vx = -(floatcos(rx,degrees) * floatsin(rz,degrees));
	vy = floatcos(rx,degrees) * floatcos(rz,degrees);
	vz = floatsin(rx,degrees);

	return 1;
}

stock static Float:floatangledistdir(Float:firstAngle, Float:secondAngle) // Ranging from -180 to 180 (directional)
{
	new Float:difference = secondAngle - firstAngle;
	while(difference < -180.0) difference += 360.0;
	while(difference > 180.0) difference -= 360.0;
	return difference;
}

stock static GetVehicleModelType(modelid)
{
	if(modelid < 400 || modelid > 611) return VEHICLE_NONE;
	return gs_vehicleTypes[modelid-400];
}

stock static IsVehicleOccupied(vehicleid)
{
	if(!vehicleid) return 0;

	foreach(new playerid : Character) if(GetPlayerVehicleID(playerid) == vehicleid) return 1;

	return 0;
}

// -----------------------------------------------------------------------------------------

// EOF
