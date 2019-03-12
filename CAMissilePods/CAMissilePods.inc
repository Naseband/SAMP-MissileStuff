#if defined __CA_M_PODS_INCLUDED_
#endinput
#endif
#define __CA_M_PODS_INCLUDED_

// ---------------------------------------------------------- Config/Limits

#define MAX_MISSILE_PODS			100

// ---------------------------------------------------------- Modes, Types, etc

// ---------------------------------------------------------- Data

// This defines the offset and rotation offset for the missile launch vector.
// For example, if you have to attach an object with a rotation of 90 degrees on the Z axis for it to face the front of the vehicle,
// set the Z rotation here to -90.0. That way the missile will launch in the correct direction.
// If a model does not need to be rotated to face the correct direction, you don't have to add it here.

enum E_MISSILE_POD_M_INFO
{
	mpinfoModelID,			// Model ID to match

	Float:mpinfoLaunchX,	// Launch Position offset for the object
	Float:mpinfoLaunchY,
	Float:mpinfoLaunchZ,

	Float:mpinfoLaunchRX,	// Launch Angle offset for the object
	Float:mpinfoLaunchRY,
	Float:mpinfoLaunchRZ
};
new gMissilePodModelInfo[][E_MISSILE_POD_M_INFO] = 
{
	{3790, 	0.0, 0.0, 0.0,	0.0, 0.0, -270.0}, // Small Missile 
	{3786, 	0.0, 0.0, 0.0,	0.0, 0.0, -270.0}, // Medium Missile
	{1636, 	0.0, 0.0, 0.0,	270.0, 0.0, 0.0} // Bomb
};

enum E_MISSILE_POD
{
	bool:mpodExists,

	mpodModelID,
	mpodModelType,

	mpodVehicleID,
	mpodVirtualWorld,
	mpodInterior,
	mpodTriggerKey,

	mpodMissileInterval,
	mpodMissileAmmo,
	mpodMissileCapacity,

	mpodMissileMode,
	mpodMissileTTL,
	Float:mpodMissileSpeed,
	Float:mpodMissileJitter,
	Float:mpodMissileSkill,
	Float:mpodMissileDownForce,

	Float:mpodAttX,
	Float:mpodAttY,
	Float:mpodAttZ,
	Float:mpodAttRX,
	Float:mpodAttRY,
	Float:mpodAttRZ,

	mpodLastFireTS,
	mpodObjectID
};
new static gMissilePods[MAX_MISSILE_PODS][E_MISSILE_POD];

new static bool:gCAMissilePodsInit;

// ---------------------------------------------------------- Forwarding

// ---------------------------------------------------------- Hooks

public OnFilterScriptInit()
{
	MPODS_Init();


	#if defined MPODS_OnFilterScriptInit
		return MPODS_OnFilterScriptInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif

#define OnFilterScriptInit MPODS_OnFilterScriptInit
#if defined MPODS_OnFilterScriptInit
	forward MPODS_OnFilterScriptInit();
#endif

// -----------------

public OnFilterScriptExit()
{
	MPODS_Exit();


	#if defined MPODS_OnFilterScriptExit
		return MPODS_OnFilterScriptExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnFilterScriptExit
	#undef OnFilterScriptExit
#else
	#define _ALS_OnFilterScriptExit
#endif

#define OnFilterScriptExit MPODS_OnFilterScriptExit
#if defined MPODS_OnFilterScriptExit
	forward MPODS_OnFilterScriptExit();
#endif

// -----------------

public OnGameModeInit()
{
	MPODS_Init();


	#if defined MPODS_OnGameModeInit
		return MPODS_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit MPODS_OnGameModeInit
#if defined MPODS_OnGameModeInit
	forward MPODS_OnGameModeInit();
#endif

// -----------------

public OnGameModeExit()
{
	MPODS_Exit();

	
	#if defined MPODS_OnGameModeExit
		return MPODS_OnGameModeExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeExit
	#undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif

#define OnGameModeExit MPODS_OnGameModeExit
#if defined MPODS_OnGameModeExit
	forward MPODS_OnGameModeExit();
#endif

// -----------------

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	new vid;

	if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER && (vid = GetPlayerVehicleID(playerid)) > 0)
	{
		new podid = -1;

		for(new id = 0; id < MAX_MISSILE_PODS; id ++) if(IsValidMissilePod(id) && GetMissilePodAmmo(id) && GetMissilePodVehicleID(id) == vid)
		{
			new trigger_key = GetMissilePodTriggerKey(id);

			if(trigger_key && (newkeys & trigger_key) && !(oldkeys & trigger_key) && IsMissilePodReady(id))
			{
				podid = id;
				break;
			}
		}

		if(podid != -1) FireMissilePod(podid);
	}

	
	#if defined MPODS_OnPlayerKeyStateChange
		return MPODS_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnPlayerKeyStateChange
	#undef OnPlayerKeyStateChange
#else
	#define _ALS_OnPlayerKeyStateChange
#endif

#define OnPlayerKeyStateChange MPODS_OnPlayerKeyStateChange
#if defined MPODS_OnPlayerKeyStateChange
	forward MPODS_OnPlayerKeyStateChange(playerid, newkeys, oldkeys);
#endif


// ---------------------------------------------------------- Internal

static stock MPODS_Init()
{
	if(gCAMissilePodsInit) return 0;

	gCAMissilePodsInit = true;

	return 1;
}

static stock MPODS_Exit()
{
	if(!gCAMissilePodsInit) return 0;

	DestroyAllMissilePods();

	gCAMissilePodsInit = false;

	return 1;
}

// ---------------------------------------------------------- External

stock CreateMissilePod(	modelid, interval, capacity,
						trigger_key = 0,
						missile_mode = MISSILE_MODE_RPG,
						missile_ttl = 5000,
						Float:missile_speed = 80.0,
						Float:missile_jitter = 0.0,
						Float:missile_skill = 0.1,
						Float:missile_down_force = 0.03)
{
	if(!gCAMissilePodsInit) return -1;

	new id = -1;

	for(new i = 0; i < MAX_MISSILE_PODS; i ++) if(!gMissilePods[i][mpodExists])
	{
		id = i;
		break;
	}

	if(id == -1) return -1;

	new model_type = -1;

	for(new i = 0; i < sizeof(gMissilePodModelInfo); i ++) if(gMissilePodModelInfo[i][mpinfoModelID] == modelid)
	{
		model_type = i;
		break;
	}

	gMissilePods[id][mpodExists] = true;

	gMissilePods[id][mpodModelID] = modelid;
	gMissilePods[id][mpodModelType] = model_type;

	gMissilePods[id][mpodVehicleID] = 0;

	gMissilePods[id][mpodMissileInterval] = interval;
	gMissilePods[id][mpodMissileAmmo] = capacity;
	gMissilePods[id][mpodMissileCapacity] = capacity;

	gMissilePods[id][mpodTriggerKey] = trigger_key;

	gMissilePods[id][mpodMissileMode] = missile_mode;
	gMissilePods[id][mpodMissileTTL] = missile_ttl;
	gMissilePods[id][mpodMissileSpeed] = missile_speed;
	gMissilePods[id][mpodMissileJitter] = missile_jitter;
	gMissilePods[id][mpodMissileSkill] = missile_skill;
	gMissilePods[id][mpodMissileDownForce] = missile_down_force;

	gMissilePods[id][mpodLastFireTS] = GetTickCount();
	gMissilePods[id][mpodObjectID] = -1;

	printf("ammo: %d cap: %d key: %d", GetMissilePodAmmo(id), GetMissilePodCapacity(id), GetMissilePodTriggerKey(id));

	return id;
}

stock DestroyMissilePod(id)
{
	if(!IsValidMissilePod(id)) return 0;

	if(IsValidDynamicObject(gMissilePods[id][mpodObjectID])) DestroyDynamicObject(gMissilePods[id][mpodObjectID]);
	gMissilePods[id][mpodObjectID] = -1;

	gMissilePods[id][mpodExists] = false;

	return 1;
}

stock DestroyAllMissilePods()
{
	for(new id = 0; id < MAX_MISSILE_PODS; id ++) DestroyMissilePod(id);
}

stock IsValidMissilePod(id)
{
	if(id < 0 || id >= MAX_MISSILE_PODS) return 0;

	return gMissilePods[id][mpodExists];
}

// -----------------

stock GetMissilePodVehicleID(id)
{
	return gMissilePods[id][mpodVehicleID];
}

stock AttachMissilePodToVehicle(id, vehicleid, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, virtual_world = 0, interior = 0)
{
	if(!IsValidMissilePod(id) || !GetVehicleModel(vehicleid)) return 0;

	new objectid = gMissilePods[id][mpodObjectID];

	if(IsValidDynamicObject(objectid)) DestroyDynamicObject(objectid);

	objectid = CreateDynamicObject(gMissilePods[id][mpodModelID], 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, gMissilePods[id][mpodVirtualWorld], gMissilePods[id][mpodInterior], -1, 150.0, 150.0);

	gMissilePods[id][mpodObjectID] = objectid;
	gMissilePods[id][mpodVirtualWorld] = virtual_world;
	gMissilePods[id][mpodInterior] = interior;

	gMissilePods[id][mpodAttX] = x;
	gMissilePods[id][mpodAttY] = y;
	gMissilePods[id][mpodAttZ] = z;
	gMissilePods[id][mpodAttRX] = rx;
	gMissilePods[id][mpodAttRY] = ry;
	gMissilePods[id][mpodAttRZ] = rz;

	AttachDynamicObjectToVehicle(objectid, vehicleid, x, y, z, rx, ry, rz);

	gMissilePods[id][mpodVehicleID] = vehicleid;
	gMissilePods[id][mpodLastFireTS] = GetTickCount();

	return 1;
}

// -----------------

stock FireMissilePod(id, target_type = -1, target_id = -1, Float:target_x = 0.0, Float:target_y = 0.0, Float:target_z = 0.0)
{
	if(!IsValidMissilePod(id) || !gMissilePods[id][mpodVehicleID]) return 0;

	new vid = gMissilePods[id][mpodVehicleID],
		model_type = gMissilePods[id][mpodModelType],
		Float:att_x = gMissilePods[id][mpodAttX],
		Float:att_y = gMissilePods[id][mpodAttY],
		Float:att_z = gMissilePods[id][mpodAttZ],
		Float:att_rx = gMissilePods[id][mpodAttRX],
		Float:att_ry = gMissilePods[id][mpodAttRY],
		Float:att_rz = gMissilePods[id][mpodAttRZ],
		Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz,
		Float:vx, Float:vy, Float:vz,
		Float:matrix[4][4];

	if(model_type != -1)
	{
		att_x += gMissilePodModelInfo[model_type][mpinfoLaunchX];
		att_y += gMissilePodModelInfo[model_type][mpinfoLaunchY];
		att_z += gMissilePodModelInfo[model_type][mpinfoLaunchZ];
		att_rx += gMissilePodModelInfo[model_type][mpinfoLaunchRX];
		att_ry += gMissilePodModelInfo[model_type][mpinfoLaunchRY];
		att_rz += gMissilePodModelInfo[model_type][mpinfoLaunchRZ];
	}

	GetVehicleObjectPositionWorld(vid, att_x, att_y, att_z, att_rx, att_ry, att_rz, x, y, z, rx, ry, rz);
	GetRotationMatrixFromEuler(matrix, rx, ry, rz, euler_samp);
	MatrixRotate(matrix, 0.0, 1.0, 0.0, 0.0, vx, vy, vz);

	FireMissile(x, y, z, vx, vy, vz,
				gMissilePods[id][mpodMissileMode],
				gMissilePods[id][mpodMissileTTL],
				gMissilePods[id][mpodMissileSpeed],
				1.5, // Step
				gMissilePods[id][mpodMissileJitter],
				gMissilePods[id][mpodMissileSkill],
				gMissilePods[id][mpodMissileDownForce],
				target_type, target_id, target_x, target_y, target_z);

	gMissilePods[id][mpodLastFireTS] = GetTickCount();

	return 1;
}

// -----------------

stock IsMissilePodReady(id)
{
	if(!GetVehicleModel(gMissilePods[id][mpodVehicleID]) || GetTickCount() - gMissilePods[id][mpodLastFireTS] < gMissilePods[id][mpodMissileInterval]) return 0;

	return 1;
}

// -----------------

stock GetMissilePodTriggerKey(id)
{
	return gMissilePods[id][mpodTriggerKey];
}

stock SetMissilePodTriggerKey(id, key)
{
	gMissilePods[id][mpodTriggerKey] = key;

	return 1;
}

// -----------------

stock GetMissilePodAmmo(id)
{
	return gMissilePods[id][mpodMissileAmmo];
}

stock GetMissilePodCapacity(id)
{
	return gMissilePods[id][mpodMissileCapacity];
}

stock SetMissilePodAmmo(id, missile_ammo)
{
	if(missile_ammo < 0) gMissilePods[id][mpodMissileAmmo] = 0;
	else if(missile_ammo > gMissilePods[id][mpodMissileCapacity]) gMissilePods[id][mpodMissileAmmo] = missile_ammo;
	else gMissilePods[id][mpodMissileAmmo] = missile_ammo;

	return 1;
}

stock SetMissilePodCapacity(id, missile_capacity)
{
	if(missile_capacity < 0) gMissilePods[id][mpodMissileCapacity] = 0;
	else gMissilePods[id][mpodMissileCapacity] = missile_capacity;

	if(gMissilePods[id][mpodMissileAmmo] > gMissilePods[id][mpodMissileCapacity]) gMissilePods[id][mpodMissileAmmo] = gMissilePods[id][mpodMissileCapacity];

	return 1;
}

stock GiveMissilePodAmmo(id, missile_ammo)
{
	gMissilePods[id][mpodMissileAmmo] += missile_ammo;

	if(gMissilePods[id][mpodMissileAmmo] < 0) gMissilePods[id][mpodMissileAmmo] = 0;
	else if(missile_ammo > gMissilePods[id][mpodMissileCapacity]) gMissilePods[id][mpodMissileAmmo] = gMissilePods[id][mpodMissileCapacity];

	return 1;
}

// -----------------

// EOF