#if defined __AA_TURRETS_INCLUDED__
#endinput
#endif
#define __AA_TURRETS_INCLUDED__

/*

Player controlled AA Turrets by NaS (c) 2021

Turrets are created via the CreateAATurret function. From there, you need to manage players getting in/out
 using PutPlayerInAATurret and RemovePlayerFromAATurret or ClearAATurret.

The turrets have a limited rotation speed, while looking around it will rotate where the camera is aiming.
When using the fire key (KEY_FIRE) the script will call OnAATurretFire at the turret's fire rate repeatedly.

The default values are below and can be redefined before including this include, but you can also set most of them per-turret.

In the callback you must spawn a projectile yourself. My CAMissile include will work fine against
 players and NPCs (including FCNPC), see the example. It is also capable of firing homing missiles.
If you prefer rotational angles instead of a vector, simply use GetAATurretCurrentRotation inside the callback.

Also it is not strictly against aircraft. You can fire downwards/horizontally too, if the max. rotation values allow it.

Originally made for my Contact Missions gamemode (base defender mission line).

*/

// ------------------------------------------------------------ Default Config

#if !defined MAX_AA_TURRETS
#define MAX_AA_TURRETS 						16
#endif

#if !defined AA_TURRET_UPDATE_INTERVAL
#define AA_TURRET_UPDATE_INTERVAL			60 // ms
#endif

#if !defined AA_TURRET_FIRE_RATE
#define AA_TURRET_FIRE_RATE 				2.0 // rounds per second, float
#endif

#if !defined AA_TURRET_ROT_SPEED_X
#define AA_TURRET_ROT_SPEED_X 				30.0 // degrees per second, float
#endif

#if !defined AA_TURRET_ROT_SPEED_Z
#define AA_TURRET_ROT_SPEED_Z 				50.0 // degrees per second, float
#endif

#if !defined AA_TURRET_MAX_ROT_X
#define AA_TURRET_MAX_ROT_X					30.0 // deg, float
#endif

#if !defined AA_TURRET_MAX_ROT_Z
#define AA_TURRET_MAX_ROT_Z 				360.0 // deg - 360.0 or more = endless rotation, float
#endif

#if !defined AA_TURRET_CAM_OFFSET_Z
#define AA_TURRET_CAM_OFFSET_Z				1.6
#endif

#if !defined AA_TURRET_VISIBLE
#define AA_TURRET_VISIBLE					true // If false, the actual turret will be invisible while controlling
#endif

#if !defined AATURRET_CROSSHAIR_COLOR
#define AATURRET_CROSSHAIR_COLOR 			0xFFFFFFFF // 0 for no crosshair
#endif

// ------------------------------------------------------------ Forward Decl

// Callbacks
forward OnAATurretFire(id, playerid, Float:x, Float:y, Float:z, Float:vx, Float:vy, Float:vz);

// Used internally
forward AATURRET_Fire(id);
forward AATURRET_UpdateRot(id);

// ------------------------------------------------------------ Internal

#if defined _FCNPC_included
#if !defined AA_TURRET_NO_FCNPC
#define AA_TURRET_FCNPC
#endif
#endif

#if !defined KEY_AIM
#define KEY_AIM 							128
#define AAT_KEY_AIM_DEFINED
#endif

// ------------------------------------------------------------ Data

enum E_AA_TURRET
{
	// Main
	bool:aatUsed,
	bool:aatActive,
	bool:aatShouldFire, // If true the turret will fire at the given firerate, if false it will terminate the timer on the next tick
	aatPlayerID,

	// Settings/Configuration vars
	aatUpdateInterval,
	Float:aatFireRate,
	Float:aatRotSpeedX,
	Float:aatRotSpeedZ,
	Float:aatMaxRotX,
	Float:aatMaxRotZ,

	// Position
	Float:aatBaseX,
	Float:aatBaseY,
	Float:aatBaseZ,
	Float:aatBaseRX,
	Float:aatBaseRY,
	Float:aatBaseRZ,
	aatBaseVW,
	aatBaseInterior,

	// Item IDs
	aatObjectIDBottom, // Streamer
	aatObjectIDTop, // Streamer
	aatObjectIDCam, // Per-player
	PlayerText3D:aatCrosshairLabelID, // Per-player
	aatUpdateTimerID,
	aatFireTimerID,

	// Internal
	aatLastUpdateTick,
	aatLastFireTick,
	Float:aatAimRX, // Offsets to BaseRX/RZ
	Float:aatAimRZ,
	Float:aatAimVX,
	Float:aatAimVY,
	Float:aatAimVZ
};

static
	gAATurrets[MAX_AA_TURRETS][E_AA_TURRET],
	gPlayerAATurret[MAX_PLAYERS] = {-1, ...},
	gPlayerAATurretPrevState[MAX_PLAYERS];

// ------------------------------------------------------------ Auto Init/Exit

static
	bool:gAATurretsInit = false,
	bool:gAATurretsInitFS = false;

// ------------------------------------------------------------ Hooks

public OnGameModeInit()
{
	AATURRET_Init();

	#if defined AATURRET_OnGameModeInit
		return AATURRET_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit AATURRET_OnGameModeInit
#if defined AATURRET_OnGameModeInit
	forward AATURRET_OnGameModeInit();
#endif

// ---

public OnFilterScriptInit()
{
	AATURRET_Init();

	gAATurretsInitFS = true;

	#if defined AATURRET_OnFilterScriptInit
		return AATURRET_OnFilterScriptInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif

#define OnFilterScriptInit AATURRET_OnFilterScriptInit
#if defined AATURRET_OnFilterScriptInit
	forward AATURRET_OnFilterScriptInit();
#endif

// ---

public OnGameModeExit()
{
	if(!gAATurretsInitFS)
		AATURRET_Exit();

	#if defined AATURRET_OnGameModeExit
		return AATURRET_OnGameModeExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeExit
	#undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif

#define OnGameModeExit AATURRET_OnGameModeExit
#if defined AATURRET_OnGameModeExit
	forward AATURRET_OnGameModeExit();
#endif

// ---

public OnFilterScriptExit()
{
	if(gAATurretsInitFS)
		AATURRET_Exit();

	#if defined AATURRET_OnFilterScriptExit
		return AATURRET_OnFilterScriptExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnFilterScriptExit
	#undef OnFilterScriptExit
#else
	#define _ALS_OnFilterScriptExit
#endif

#define OnFilterScriptExit AATURRET_OnFilterScriptExit
#if defined AATURRET_OnFilterScriptExit
	forward AATURRET_OnFilterScriptExit();
#endif

// ---

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	new id;

	if(IsValidAATurret((id = GetPlayerAATurretID(playerid))))
	{
		if(newkeys & KEY_FIRE && !(oldkeys & KEY_FIRE)) // Press
		{
			gAATurrets[id][aatShouldFire] = true;

			if(gAATurrets[id][aatFireTimerID] == -1)
			{
				gAATurrets[id][aatFireTimerID] = SetTimerEx("AATURRET_Fire", floatround(1000.0 / gAATurrets[id][aatFireRate]), 1, "i", id);
				AATURRET_Fire(id);
			}
		}
		else if(oldkeys & KEY_FIRE && !(newkeys & KEY_FIRE)) // Release
		{
			gAATurrets[id][aatShouldFire] = false;
		}
	}

	#if defined AATURRET_OnPlayerKeyStateChange
		return AATURRET_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerKeyStateChange
	#undef OnPlayerKeyStateChange
#else
	#define _ALS_OnPlayerKeyStateChange
#endif

#define OnPlayerKeyStateChange AATURRET_OnPlayerKeyStateChange
#if defined AATURRET_OnPlayerKeyStateChange
	forward AATURRET_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
#endif

// ---

public OnPlayerStateChange(playerid, newstate, oldstate)
{
	if(IsValidAATurret(GetPlayerAATurretID(playerid)))
	{
		if(newstate != PLAYER_STATE_SPECTATING && oldstate == PLAYER_STATE_SPECTATING)
		{
			RemovePlayerFromAATurret(playerid);
		}
	}

	#if defined AATURRET_OnPlayerStateChange
		return AATURRET_OnPlayerStateChange(playerid, newstate, oldstate);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerStateChange
	#undef OnPlayerStateChange
#else
	#define _ALS_OnPlayerStateChange
#endif

#define OnPlayerStateChange AATURRET_OnPlayerStateChange
#if defined AATURRET_OnPlayerStateChange
	forward AATURRET_OnPlayerStateChange(playerid, newstate, oldstate);
#endif

// ------------------------------------------------------------ Init/Exit

AATURRET_Init()
{
	if(gAATurretsInit)
		return;

	gAATurretsInit = true;
}

AATURRET_Exit()
{
	if(!gAATurretsInit)
		return;

	for(new i = 0; i < MAX_AA_TURRETS; ++i)
		DestroyAATurret(i);

	gAATurretsInit = false;
}

// ------------------------------------------------------------ Internal Funcs

// Gets X/Z angles (yaw and pitch) from the player's camera
static stock AATURRET_GetPlayerCameraAngles(playerid, &Float:rx, &Float:rz)
{
	new Float:vx, Float:vy, Float:vz, r = GetPlayerCameraFrontVector(playerid, vx, vy, vz);

	rx = -(acos(vz) - 90.0);
	rz = atan2(vy, vx) - 90.0;

	return r;
}

// Returns the directional difference between two angles
static stock Float:AATURRET_RetAngleDistance(Float:firstAngle, Float:secondAngle)
{
	new Float:difference = secondAngle - firstAngle;
	while(difference < -180.0) difference += 360.0;
	while(difference > 180.0) difference -= 360.0;
	return difference;
}

// This function updates the aiming angles depending on the controlling player's camera vector (if any, otherwise resets).
static stock AATURRET_Update(id)
{
	if(!IsValidAATurret(id))
		return 0;

	new tick = GetTickCount(),
		Float:time_passed = float(tick - gAATurrets[id][aatLastUpdateTick]) / 1000.0,
		playerid = gAATurrets[id][aatPlayerID],
		Float:target_rx, Float:target_rz,
		Float:max_rot_x = gAATurrets[id][aatMaxRotX],
		Float:max_rot_z = gAATurrets[id][aatMaxRotZ],
		Float:dif_rx, Float:dif_rz,
		Float:dif_rx2, Float:dif_rz2;

	if(time_passed < 0.0015)
		return 1;

	if(playerid != -1)
		AATURRET_GetPlayerCameraAngles(playerid, target_rx, target_rz);

	while(target_rx < 0.0)
		target_rx += 360.0;
	while(target_rx >= 360.0)
		target_rx -= 360.0;

	while(target_rz < 0.0)
		target_rz += 360.0;
	while(target_rz >= 360.0)
		target_rz -= 360.0;

	// Get angle difference (and direction)

	dif_rx = AATURRET_RetAngleDistance(gAATurrets[id][aatAimRX], target_rx);
	dif_rz = AATURRET_RetAngleDistance(gAATurrets[id][aatAimRZ], target_rz);

	if(playerid == -1 && floatabs(dif_rx) < 0.2 && floatabs(dif_rz) < 0.2) // No player is inside and the turret has reset
	{
		return 0;
	}

	// Limit rotation to turret's rot speed and normalize

	if(dif_rx * time_passed > time_passed * gAATurrets[id][aatRotSpeedX])
		target_rx = gAATurrets[id][aatAimRX] + time_passed * gAATurrets[id][aatRotSpeedX];
	else if(dif_rx * time_passed < -time_passed * gAATurrets[id][aatRotSpeedX])
		target_rx = gAATurrets[id][aatAimRX] + time_passed * -gAATurrets[id][aatRotSpeedX];
	else 
		target_rx = gAATurrets[id][aatAimRX] + time_passed * dif_rx;
	
	if(dif_rz * time_passed > time_passed * gAATurrets[id][aatRotSpeedZ])
		target_rz = gAATurrets[id][aatAimRZ] + time_passed * gAATurrets[id][aatRotSpeedZ];
	else if(dif_rz * time_passed < -time_passed * gAATurrets[id][aatRotSpeedZ])
		target_rz = gAATurrets[id][aatAimRZ] + time_passed * -gAATurrets[id][aatRotSpeedZ];
	else
		target_rz = gAATurrets[id][aatAimRZ] + time_passed * dif_rz;

	while(target_rx < 0.0)
		target_rx += 360.0;
	while(target_rx >= 360.0)
		target_rx -= 360.0;

	while(target_rz < 0.0)
		target_rz += 360.0;
	while(target_rz >= 360.0)
		target_rz -= 360.0;

	gAATurrets[id][aatAimRX] = target_rx;
	gAATurrets[id][aatAimRZ] = target_rz;
	gAATurrets[id][aatLastUpdateTick] = tick;

	gAATurrets[id][aatAimVX] = -floatcos(target_rx, degrees) * floatsin(target_rz, degrees);
	gAATurrets[id][aatAimVY] = floatcos(target_rx, degrees) * floatcos(target_rz, degrees);
	gAATurrets[id][aatAimVZ] = floatsin(target_rx, degrees);

	return 1;
}

// Updates object rotation and crosshair. If no player is inside and the turret has finished moving, AATURRET_Update will return 0 and the timer terminated
public AATURRET_UpdateRot(id)
{
	if(AATURRET_Update(id))
	{
		SetDynamicObjectRot(gAATurrets[id][aatObjectIDBottom], gAATurrets[id][aatBaseRX], gAATurrets[id][aatBaseRY], gAATurrets[id][aatBaseRZ] + gAATurrets[id][aatAimRZ]);
		SetDynamicObjectRot(gAATurrets[id][aatObjectIDTop], gAATurrets[id][aatBaseRX] + gAATurrets[id][aatAimRX] - 30.0, gAATurrets[id][aatBaseRY], gAATurrets[id][aatBaseRZ] + gAATurrets[id][aatAimRZ]);

		new playerid;

		if((playerid = gAATurrets[id][aatPlayerID]) != -1)
		{
			if(gAATurrets[id][aatCrosshairLabelID] != PlayerText3D:-1)
				DeletePlayer3DTextLabel(playerid, gAATurrets[id][aatCrosshairLabelID]);

			gAATurrets[id][aatCrosshairLabelID] = CreatePlayer3DTextLabel(playerid, "X", AATURRET_CROSSHAIR_COLOR, gAATurrets[id][aatBaseX] + 30.0 * gAATurrets[id][aatAimVX], gAATurrets[id][aatBaseY] + 30.0 * gAATurrets[id][aatAimVY], gAATurrets[id][aatBaseZ] + AA_TURRET_CAM_OFFSET_Z + 30.0 * gAATurrets[id][aatAimVZ], 50.0);
		}
	}
	else
	{
		AATURRET_ResetTimers(id);
	}

	return 1;
}

// Triggers le callback. If called when the key has been released, terminates the timer.
public AATURRET_Fire(id)
{
	if(!IsValidAATurret(id))
		return 0;

	new playerid = gAATurrets[id][aatPlayerID];

	if(playerid == -1)
	{
		AATURRET_ResetTimers(id);

		return 0;
	}

	AATURRET_Update(id);

	if(!gAATurrets[id][aatShouldFire])
	{
		if(gAATurrets[id][aatFireTimerID] != -1)
		{
			KillTimer(gAATurrets[id][aatFireTimerID]);
			gAATurrets[id][aatFireTimerID] = -1;
		}

		return 0;
	}

	// Basically get vector and call OnAATurretFire callback.

	new Float:x = gAATurrets[id][aatBaseX],
		Float:y = gAATurrets[id][aatBaseY],
		Float:z = gAATurrets[id][aatBaseZ] + AA_TURRET_CAM_OFFSET_Z,
		Float:vx, Float:vy, Float:vz;

	GetAATurretCurrentAimVectors(id, vx, vy, vz);

	x += vx;
	y += vy;
	z += vz;

	CallLocalFunction("OnAATurretFire", "iiffffff", id, playerid, x, y, z, vx, vy, vz);

	gAATurrets[id][aatLastFireTick] = GetTickCount();

	return 1;
}

// Resets timers...
static stock AATURRET_ResetTimers(id)
{
	if(gAATurrets[id][aatUpdateTimerID] != -1)
	{
		KillTimer(gAATurrets[id][aatUpdateTimerID]);
		gAATurrets[id][aatUpdateTimerID] = -1;
	}

	if(gAATurrets[id][aatFireTimerID] != -1)
	{
		KillTimer(gAATurrets[id][aatFireTimerID]);
		gAATurrets[id][aatFireTimerID] = -1;
	}

	gAATurrets[id][aatShouldFire] = false;

	return 1;
}

// ------------------------------------------------------------ External Funcs

stock IsValidAATurret(id)
{
	return (id >= 0 && id < MAX_AA_TURRETS && gAATurrets[id][aatUsed]);
}

stock CreateAATurret(Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, virtual_world, interior, Float:fire_rate = AA_TURRET_FIRE_RATE, Float:rot_speed_x = AA_TURRET_ROT_SPEED_X, Float:rot_speed_z = AA_TURRET_ROT_SPEED_Z, Float:max_rot_x = AA_TURRET_MAX_ROT_X, Float:max_rot_z = AA_TURRET_MAX_ROT_Z, update_interval = AA_TURRET_UPDATE_INTERVAL)
{
	new id = -1;

	for(new i = 0; i != MAX_AA_TURRETS; ++i)
	{
		if(!IsValidAATurret(id))
		{
			id = i;
			break;
		}
	}

	if(id == -1)
		return -1;

	gAATurrets[id][aatUsed] = true;
	gAATurrets[id][aatActive] = true;
	gAATurrets[id][aatShouldFire] = false;
	gAATurrets[id][aatPlayerID] = -1;

	gAATurrets[id][aatUpdateInterval] = update_interval;
	gAATurrets[id][aatFireRate] = fire_rate;
	gAATurrets[id][aatRotSpeedX] = rot_speed_x;
	gAATurrets[id][aatRotSpeedZ] = rot_speed_z;
	gAATurrets[id][aatMaxRotX] = max_rot_x;
	gAATurrets[id][aatMaxRotZ] = max_rot_z;

	gAATurrets[id][aatBaseX] = x;
	gAATurrets[id][aatBaseY] = y;
	gAATurrets[id][aatBaseZ] = z;
	gAATurrets[id][aatBaseRX] = rx;
	gAATurrets[id][aatBaseRY] = ry;
	gAATurrets[id][aatBaseRZ] = rz;
	gAATurrets[id][aatBaseVW] = virtual_world;
	gAATurrets[id][aatBaseInterior] = interior;

	new objectid = gAATurrets[id][aatObjectIDBottom] = CreateDynamicObject(18848, x, y, z, rx, ry, rz, virtual_world, interior, -1, 900.0, 900.0);
	SetDynamicObjectMaterial(objectid, 0, 10101, "2notherbuildsfe", "Bow_Abpave_Gen", 0x00000001); // Make bottom object's main launcher invisible
	SetDynamicObjectMaterial(objectid, 1, 10101, "2notherbuildsfe", "Bow_Abpave_Gen", 0x00000001);
	SetDynamicObjectMaterial(objectid, 2, 10101, "2notherbuildsfe", "Bow_Abpave_Gen", 0x00000001);
	SetDynamicObjectMaterial(objectid, 4, 10101, "2notherbuildsfe", "Bow_Abpave_Gen", 0x00000001);

	objectid = gAATurrets[id][aatObjectIDTop] = CreateDynamicObject(18848, x, y, z, rx, ry, rz, virtual_world, interior, -1, 900.0, 900.0);
	SetDynamicObjectMaterial(objectid, 3, 10101, "2notherbuildsfe", "Bow_Abpave_Gen", 0x00000001); // Make top object's base invisible
	SetDynamicObjectMaterial(objectid, 5, 10101, "2notherbuildsfe", "Bow_Abpave_Gen", 0x00000001);

	gAATurrets[id][aatCrosshairLabelID] = PlayerText3D:-1;

	gAATurrets[id][aatUpdateTimerID] = -1;
	gAATurrets[id][aatFireTimerID] = -1;

	gAATurrets[id][aatAimRX] = 0.0;
	gAATurrets[id][aatAimRZ] = 0.0;
	gAATurrets[id][aatAimVX] = 0.0;
	gAATurrets[id][aatAimVY] = 0.0;
	gAATurrets[id][aatAimVZ] = 0.0;

	gAATurrets[id][aatLastUpdateTick] = gAATurrets[id][aatLastFireTick] = GetTickCount();

	return id;
}

stock DestroyAATurret(id)
{
	if(!IsValidAATurret(id))
		return 0;

	ClearAATurret(id);

	DestroyDynamicObject(gAATurrets[id][aatObjectIDBottom]);
	DestroyDynamicObject(gAATurrets[id][aatObjectIDTop]);
	gAATurrets[id][aatUsed] = false;

	AATURRET_ResetTimers(id);

	return 1;
}

stock GetAATurretPlayerID(id)
{
	return gAATurrets[id][aatPlayerID];
}

stock GetPlayerAATurretID(playerid)
{
	return gPlayerAATurret[playerid];
}

stock PutPlayerInAATurret(playerid, id)
{
	if(!IsPlayerConnected(playerid) || IsPlayerNPC(playerid) || GetAATurretPlayerID(id) != -1)
		return 0;

	if(gPlayerAATurret[playerid] != -1)
		RemovePlayerFromAATurret(playerid);

	gPlayerAATurret[playerid] = id;
	gAATurrets[id][aatPlayerID] = playerid;

	gAATurrets[id][aatLastFireTick] = gAATurrets[id][aatLastUpdateTick] = GetTickCount();

	AATURRET_ResetTimers(id);
	gAATurrets[id][aatUpdateTimerID] = SetTimerEx("AATURRET_UpdateRot", gAATurrets[id][aatUpdateInterval], 1, "i", id);

	if((gPlayerAATurretPrevState[playerid] = GetPlayerState(playerid)) != PLAYER_STATE_SPECTATING)
	{
		// Save Player Health, Armor and Weapons

		TogglePlayerSpectating(playerid, 1);
	}

	// Create invisible obj and attach the cam
	gAATurrets[id][aatObjectIDCam] = CreatePlayerObject(playerid, 19300, gAATurrets[id][aatBaseX], gAATurrets[id][aatBaseY], gAATurrets[id][aatBaseZ] + AA_TURRET_CAM_OFFSET_Z, 0.0, 0.0, 0.0);
	AttachCameraToPlayerObject(playerid, gAATurrets[id][aatObjectIDCam]);

	#if !AA_TURRET_VISIBLE
	Streamer_ToggleItem(playerid, STREAMER_TYPE_OBJECT, gAATurrets[id][aatObjectIDTop], 0); // Make the top object invisible for this dude
	#endif

	return 1;
}

stock RemovePlayerFromAATurret(playerid)
{
	new id = gPlayerAATurret[playerid];

	gPlayerAATurret[playerid] = -1;

	if(!IsValidAATurret(id))
		return 0;

	if(gAATurrets[id][aatPlayerID] != -1)
	{
		gAATurrets[id][aatPlayerID] = -1;

		if(GetPlayerState(playerid) == PLAYER_STATE_SPECTATING)
			TogglePlayerSpectating(playerid, 0);

		if(gPlayerAATurretPrevState[playerid] == PLAYER_STATE_ONFOOT || gPlayerAATurretPrevState[playerid] == PLAYER_STATE_DRIVER || gPlayerAATurretPrevState[playerid] == PLAYER_STATE_PASSENGER)
		{
			// Restore player stuff
		}

		DestroyPlayerObject(playerid, gAATurrets[id][aatObjectIDCam]);

		if(gAATurrets[id][aatCrosshairLabelID] != PlayerText3D:-1)
		{
			DeletePlayer3DTextLabel(playerid, gAATurrets[id][aatCrosshairLabelID]);
			gAATurrets[id][aatCrosshairLabelID] = PlayerText3D:-1;
		}

		#if !AA_TURRET_VISIBLE
		Streamer_ToggleItem(playerid, STREAMER_TYPE_OBJECT, gAATurrets[id][aatObjectIDTop], 1); // Show it again
		#endif

		gAATurrets[id][aatShouldFire] = false;
	}

	return 1;
}

stock ClearAATurret(id)
{
	if(gAATurrets[id][aatPlayerID] != -1)
		RemovePlayerFromAATurret(gAATurrets[id][aatPlayerID]);

	return 1;
}

stock ToggleAATurretActive(id, active)
{
	if(!active)
		ClearAATurret(id);

	gAATurrets[id][aatActive] = _:!!bool:active;

	return 1;
}

stock GetAATurretCurrentAimVectors(id, &Float:vx, &Float:vy, &Float:vz)
{
	vx = gAATurrets[id][aatAimVX];
	vy = gAATurrets[id][aatAimVY];
	vz = gAATurrets[id][aatAimVZ];

	return 1;
}

stock ForceAATurretAimUpdate(id)
{
	return AATURRET_Update(id);
}

stock GetAATurretVirtualWorld(id)
{
	return gAATurrets[id][aatBaseVW];
}

stock GetAATurretInterior(id)
{
	return gAATurrets[id][aatBaseInterior];
}

// ------------------------------------------------------------ 
// ------------------------------------------------------------ 
// ------------------------------------------------------------ 

#if defined AAT_KEY_AIM_DEFINED
#undef KEY_AIM
#undef AAT_KEY_AIM_DEFINED
#endif

// EOF
