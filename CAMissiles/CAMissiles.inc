#if defined __CA_MISSILES_INCLUDED_
#endinput
#endif
#define __CA_MISSILES_INCLUDED_

// ---------------------------------------------------------- Config/Limits

#if !defined MAX_MISSILES
#define MAX_MISSILES     		100
#endif

#if !defined MISSILE_SPEED
#define MISSILE_SPEED    		75.0 // Default Missile Speed in m/s
#endif

#if !defined MISSILE_STEP
#define MISSILE_STEP			2.5 // Default Step Distance in m
#endif

#if !defined MISSILE_JITTER
#define MISSILE_JITTER			0.005 // Default Jitter in m/step
#endif

#if !defined MISSILE_SKILL
#define MISSILE_SKILL			0.12 // Default Homing Skill (0.0 = no tracking, 1.0 = instant turns)
#endif

#if !defined MISSILE_DOWN_FORCE
#define MISSILE_DOWN_FORCE 		0.02 // Default Artillery Down Force in m/step
#endif

#if !defined MISSILE_TTL
#define MISSILE_TTL				10000 // Default Time-To-Live in ms
#endif

#if !defined MISSILE_MODEL
#define MISSILE_MODEL    		345 // Default Missile Model (345 = Missile)
#endif

#if !defined BOMB_MODEL
#define BOMB_MODEL 				1636
#endif

//#define MISSILE_VEHICLE_CHECK // define this before including CAMissiles to perform vehicle hit detection (resource intensive)

// ---------------------------------------------------------- Modes, Types, etc

enum (+=1)
{
	MISSILE_MODE_ARTILLERY,
	MISSILE_MODE_RPG,
	MISSILE_MODE_HOMING,
	MISSILE_MODE_GUARDED,
	MISSILE_MODE_SWARM,

	MISSILE_MODE_MAX
};

enum (+=1)
{
	MISSILE_TARGET_POS,
	MISSILE_TARGET_PLAYER,
	MISSILE_TARGET_VEHICLE
};

enum (+=1)
{
	MISSILE_HIT_TYPE_NONE, // Died due to TTL
	MISSILE_HIT_TYPE_COLLISION, // Collided with world (static or dynamic CA Object)
	MISSILE_HIT_TYPE_PLAYER,
	MISSILE_HIT_TYPE_VEHICLE // Only available for homing missiles or if vehicle check is on
};

// ---------------------------------------------------------- Data

enum E_MISSILE
{
	bool:misUsed,

	misMode,
	misSpawnTick,
	misTTL,
	misExtra,
	misVirtualWorld,
	misInterior,
	bool:misInitCheck,

	Float:misX,
	Float:misY,
	Float:misZ,
	Float:misVX,
	Float:misVY,
	Float:misVZ,

	Float:misSpeed,
	Float:misStep,
	Float:misJitter,
	Float:misSkill,
	Float:misDownForce,

	misObjectID,

	misTargetType,
	misTargetID,
	Float:misTargetPos[3]
};
new static gMissiles[MAX_MISSILES][E_MISSILE];

new static bool:gCAMissilesInit = false;

// ---------------------------------------------------------- Forwarding public

forward OnCAMissileExplode(id, hittype, hitid, bool:swarm, Float:x, Float:y, Float:z, virtual_world, interior);

// ---------------------------------------------------------- Forwarding private

forward Float:cmiss_RetRZFromVectorXY(Float:vx, Float:vy);
forward Float:cmiss_RetRXFromVectorZ(Float:vz);
forward Float:cmiss_frandomEx(Float:min, Float:max = (Float:0x7FFFFFFF), dp = 3);

// ---------------------------------------------------------- Hooks

public OnFilterScriptInit()
{
	MISS_Init();


	#if defined MISS_OnFilterScriptInit
		return MISS_OnFilterScriptInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnFilterScriptInit
	#undef OnFilterScriptInit
#else
	#define _ALS_OnFilterScriptInit
#endif

#define OnFilterScriptInit MISS_OnFilterScriptInit
#if defined MISS_OnFilterScriptInit
	forward MISS_OnFilterScriptInit();
#endif


public OnFilterScriptExit()
{
	MISS_Exit();


	#if defined MISS_OnFilterScriptExit
		return MISS_OnFilterScriptExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnFilterScriptExit
	#undef OnFilterScriptExit
#else
	#define _ALS_OnFilterScriptExit
#endif

#define OnFilterScriptExit MISS_OnFilterScriptExit
#if defined MISS_OnFilterScriptExit
	forward MISS_OnFilterScriptExit();
#endif


public OnGameModeInit()
{
	MISS_Init();


	#if defined MISS_OnGameModeInit
		return MISS_OnGameModeInit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeInit
	#undef OnGameModeInit
#else
	#define _ALS_OnGameModeInit
#endif

#define OnGameModeInit MISS_OnGameModeInit
#if defined MISS_OnGameModeInit
	forward MISS_OnGameModeInit();
#endif


public OnGameModeExit()
{
	MISS_Exit();

	
	#if defined MISS_OnGameModeExit
		return MISS_OnGameModeExit();
	#else
		return 1;
	#endif
}
#if defined _ALS_OnGameModeExit
	#undef OnGameModeExit
#else
	#define _ALS_OnGameModeExit
#endif

#define OnGameModeExit MISS_OnGameModeExit
#if defined MISS_OnGameModeExit
	forward MISS_OnGameModeExit();
#endif


public OnObjectMoved(objectid)
{
	new id = -1;
	for(new i = 0; i < MAX_MISSILES; i ++) if(gMissiles[i][misUsed] && gMissiles[i][misObjectID] == objectid)
	{
	    id = i;
	    break;
	}
	
	if(id != -1)
	{
		ProcessMissile(id);
	}

	#if defined CMISS_OnObjectMoved
		return CMISS_OnObjectMoved(objectid);
	#else
		return 1;
	#endif
}
#if defined _ALS_OnObjectMoved
	#undef OnObjectMoved
#else
	#define _ALS_OnObjectMoved
#endif

#define OnObjectMoved CMISS_OnObjectMoved
#if defined CMISS_OnObjectMoved
	forward CMISS_OnObjectMoved(objectid);
#endif

// ---------------------------------------------------------- Internal

static stock MISS_Init()
{
	if(gCAMissilesInit) return 0;


	gCAMissilesInit = true;

	return 1;
}

static stock MISS_Exit()
{
	if(!gCAMissilesInit) return 0;

	DestroyAllMissiles();

	gCAMissilesInit = false;

	return 1;
}

// ---------------------------------------------------------- External

stock FireMissile(Float:x, Float:y, Float:z, virtual_world, interior, // Position, vw, int
			Float:vx, Float:vy, Float:vz,  // Vector (does not need to be normalized)
			mode = MISSILE_MODE_RPG, // Missile Mode
			ttl = MISSILE_TTL, // Missile time-to-live (in ms)
			Float:speed = MISSILE_SPEED,  // Missile Speed
			Float:step = MISSILE_STEP, // Missile Step Size
			Float:jitter = MISSILE_JITTER, // Missile Jitter
			Float:skill = MISSILE_SKILL, // Missile Skill (0.0 - 1.0, 0.0 = RPG, 1.0 = instant turns) - HOMING
			Float:down_force = MISSILE_DOWN_FORCE, // Missile Gravity
			target_type = -1,
			target_id = -1,
			Float:target_x = 0.0, Float:target_y = 0.0, Float:target_z = 0.0,
			extra = cellmin,
			bool:initial_check = true)
{
	if(!gCAMissilesInit) return -1;

	new id = -1;

	for(new i = 0; i < MAX_MISSILES; i ++) if(!gMissiles[i][misUsed])
	{
		id = i;
		break;
	}

	if(id == -1) return -1;

	cmiss_NormalizeVector(vx, vy, vz);

	if(VectorSize(vx, vy, vz) == 0.0) return -1;

	gMissiles[id][misMode] = mode;
	gMissiles[id][misTTL] = ttl;
	gMissiles[id][misExtra] = extra;
	gMissiles[id][misInitCheck] = initial_check;

	gMissiles[id][misX] = x;
	gMissiles[id][misY] = y;
	gMissiles[id][misZ] = z;
	gMissiles[id][misVirtualWorld] = virtual_world;
	gMissiles[id][misInterior] = interior;

	gMissiles[id][misVX] = vx;
	gMissiles[id][misVY] = vy;
	gMissiles[id][misVZ] = vz;

	gMissiles[id][misSpeed] = speed;
	gMissiles[id][misStep] = step;
	gMissiles[id][misJitter] = jitter;
	gMissiles[id][misSkill] = skill;
	gMissiles[id][misDownForce] = down_force;

	gMissiles[id][misTargetType] = target_type;
	gMissiles[id][misTargetID] = target_id,
	gMissiles[id][misTargetPos][0] = target_x;
	gMissiles[id][misTargetPos][1] = target_y;
	gMissiles[id][misTargetPos][2] = target_z;

	switch(mode)
	{
		case MISSILE_MODE_ARTILLERY: gMissiles[id][misObjectID] = CreateObject(BOMB_MODEL, x, y, z, cmiss_RetRXFromVectorZ(vz), 0.0, cmiss_RetRZFromVectorXY(vx, vy), 900.0);
		default: gMissiles[id][misObjectID] = CreateObject(MISSILE_MODEL, x, y, z, cmiss_RetRXFromVectorZ(vz), 0.0, cmiss_RetRZFromVectorXY(vx, vy), 900.0);
	}

	if(!IsValidObject(gMissiles[id][misObjectID])) return -1;

	gMissiles[id][misUsed] = true;
	gMissiles[id][misSpawnTick] = GetTickCount();

	ProcessMissile(id);

	return id;
}

stock IsValidMissile(id)
{
	if(id < 0 || id >= MAX_MISSILES || !gMissiles[id][misUsed]) return 0;

	return 1;
}

stock IsValidMissileObject(id)
{
	if(!IsValidMissile(id)) return 0;

	if(!IsValidObject(gMissiles[id][misObjectID])) return 0;

	return 1;
}

stock ExplodeMissile(id, hittype = MISSILE_HIT_TYPE_NONE, hitid = -1)
{
	if(!IsValidMissile(id)) return 0;

	switch(gMissiles[id][misMode])
	{
		case MISSILE_MODE_SWARM:
		{
			new Float:x = gMissiles[id][misX], Float:y = gMissiles[id][misY], Float:z = gMissiles[id][misZ],
				Float:vx = gMissiles[id][misVX], Float:vy = gMissiles[id][misVY], Float:vz = gMissiles[id][misVZ],
				Float:speed = gMissiles[id][misSpeed],
				Float:step = gMissiles[id][misStep],
				ttl = gMissiles[id][misTTL],
				virtual_world = gMissiles[id][misVirtualWorld], interior = gMissiles[id][misInterior];

			CreateExplosion(x, y, z, 12, 5.0);

			CallLocalFunction("OnCAMissileExplode", "iiiifff", id, hittype, hitid, 1, x, y, z);

			DestroyMissile(id);

			FireMissile(x, y, z, virtual_world, interior, vx, vy, vz, MISSILE_MODE_RPG, ttl, speed, step, 0.01, .initial_check = false);
			FireMissile(x, y, z, virtual_world, interior, vx, vy, vz, MISSILE_MODE_RPG, ttl, speed, step, 0.01, .initial_check = false);
			FireMissile(x, y, z, virtual_world, interior, vx, vy, vz, MISSILE_MODE_RPG, ttl, speed, step, 0.01, .initial_check = false);
			FireMissile(x, y, z, virtual_world, interior, vx, vy, vz, MISSILE_MODE_RPG, ttl, speed, step, 0.01, .initial_check = false);
		}

		default:
		{
			CreateExplosion(gMissiles[id][misX], gMissiles[id][misY], gMissiles[id][misZ], 10, 25.0);

			CallLocalFunction("OnCAMissileExplode", "iifffii", id, hittype, hitid, 0, gMissiles[id][misX], gMissiles[id][misY], gMissiles[id][misZ], gMissiles[id][misVirtualWorld], gMissiles[id][misInterior]);

			DestroyMissile(id);
		}
	}

	return 1;
}

stock DestroyMissile(id)
{
	if(!IsValidMissile(id)) return 0;

	if(IsValidMissileObject(id))
	{
		DestroyObject(gMissiles[id][misObjectID]);
		gMissiles[id][misObjectID] = -1;
	}

	gMissiles[id][misUsed] = false;

	return 1;
}

stock DestroyAllMissiles()
{
	for(new i = 0; i < MAX_MISSILES; i ++) DestroyMissile(i);

	return 1;
}

stock GetMissileExtra(id)
{
	if(!IsValidMissile(id)) return cellmin;

	return gMissiles[id][misExtra];
}

stock ProcessMissile(id)
{
	if(GetTickCount() - gMissiles[id][misSpawnTick] > gMissiles[id][misTTL])
	{
		ExplodeMissile(id);

		return 1;
	}

	new Float:x = gMissiles[id][misX], Float:y = gMissiles[id][misY], Float:z = gMissiles[id][misZ],
		Float:ox = x, Float:oy = y, Float:oz = z,
		Float:vx = gMissiles[id][misVX], Float:vy = gMissiles[id][misVY], Float:vz = gMissiles[id][misVZ],
		mode = gMissiles[id][misMode],
		Float:speed = gMissiles[id][misSpeed],
		Float:skill = gMissiles[id][misSkill],
		Float:step = gMissiles[id][misStep],
		Float:jitter = gMissiles[id][misJitter];

	#if defined MISSILE_VEHICLE_CHECK

	for(new i = 1; i <= MAX_VEHICLES; ++i)
	{
		if(GetVehicleModel(i))
		{
			new Float:vehx, Float:vehy, Float:vehz;
			GetVehiclePos(i, vehx, vehy, vehz);
			
			if(VectorSize(x - vehx, y - vehy, z - vehz) < 10.0)
			{
				ExplodeMissile(id, MISSILE_HIT_TYPE_VEHICLE, i);

				return 1;
			}
		}
	}

	#endif

	switch(mode)
	{
		case MISSILE_MODE_ARTILLERY:
		{
			vz -= gMissiles[id][misDownForce];

			if(jitter > 0.0)
			{
				vx += cmiss_frandomEx(-jitter, jitter);
		        vy += cmiss_frandomEx(-jitter, jitter);
		        vz += cmiss_frandomEx(-jitter, jitter);
		    }
		}

		case MISSILE_MODE_RPG:
		{
			if(jitter > 0.0)
			{
				vx += cmiss_frandomEx(-jitter, jitter);
		        vy += cmiss_frandomEx(-jitter, jitter);
		        vz += cmiss_frandomEx(-jitter, jitter);
		    }
		}

		case MISSILE_MODE_HOMING:
		{
			new target_type = gMissiles[id][misTargetType], target_id = gMissiles[id][misTargetID],
				Float:dx = vx, Float:dy = vy, Float:dz = vz;

			if(GetTickCount() - gMissiles[id][misSpawnTick] > 700)
			{
				switch(target_type)
				{
					case MISSILE_TARGET_PLAYER:
					{
						if(IsPlayerConnected(target_id))
						{
							GetPlayerPos(target_id, dx, dy, dz);

							dx = dx - x;
							dy = dy - y;
							dz = dz - z;

							if(VectorSize(dx, dy, dz) < 3.0)
							{
								ExplodeMissile(id, MISSILE_HIT_TYPE_PLAYER, target_id);

								return 1;
							}

							cmiss_NormalizeVector(dx, dy, dz);
						}
					}
					case MISSILE_TARGET_VEHICLE:
					{
						if(GetVehicleModel(target_id))
						{
							GetVehiclePos(target_id, dx, dy, dz);

							dx = dx - x;
							dy = dy - y;
							dz = dz - z;

							if(VectorSize(dx, dy, dz) < 3.0)
							{
								ExplodeMissile(id, MISSILE_HIT_TYPE_VEHICLE, target_id);

								return 1;
							}

							cmiss_NormalizeVector(dx, dy, dz);
						}
					}
					case MISSILE_TARGET_POS:
					{
						dx = gMissiles[id][misTargetPos][0] - x;
						dy = gMissiles[id][misTargetPos][1] - y;
						dz = gMissiles[id][misTargetPos][2] - z;

						if(VectorSize(dx, dy, dz) < 3.0)
						{
							ExplodeMissile(id, MISSILE_HIT_TYPE_COLLISION);

							return 1;
						}

						cmiss_NormalizeVector(dx, dy, dz);
					}
				}
			}

			vx = ((1.0 - skill) * vx) + ((skill) * dx);
   			vy = ((1.0 - skill) * vy) + ((skill) * dy);
   			vz = ((1.0 - skill) * vz) + ((skill) * dz);

   			if(jitter > 0.0)
			{
				vx += cmiss_frandomEx(-jitter, jitter);
		        vy += cmiss_frandomEx(-jitter, jitter);
		        vz += cmiss_frandomEx(-jitter, jitter);
		    }
		}

		case MISSILE_MODE_SWARM:
		{
			if(jitter > 0.0)
			{
				vx += cmiss_frandomEx(-jitter, jitter);
		        vy += cmiss_frandomEx(-jitter, jitter);
		        vz += cmiss_frandomEx(-jitter, jitter);
		    }
		}
	}

	cmiss_NormalizeVector(vx, vy, vz);

	x = x + step * vx;
	y = y + step * vy;
	z = z + step * vz;

	switch(mode)
	{
		case MISSILE_MODE_SWARM:
		{
			if(GetTickCount() - gMissiles[id][misSpawnTick] > 1000 && CA_ContactTest(13592, x, y, z, 0.0, 0.0, 0.0))
			{
				gMissiles[id][misX] = x;
				gMissiles[id][misY] = y;
				gMissiles[id][misZ] = z;

				ExplodeMissile(id);

				return 1;
			}
		}
	}

	if((!gMissiles[id][misInitCheck] || GetTickCount() - gMissiles[id][misSpawnTick] > 100) && CA_RayCastLine(ox, oy, oz, x, y, z, x, y, z) != 0)
	{
		gMissiles[id][misX] = x;
		gMissiles[id][misY] = y;
		gMissiles[id][misZ] = z;

		ExplodeMissile(id, MISSILE_HIT_TYPE_COLLISION);

		return 1;
	}

	gMissiles[id][misX] = x;
	gMissiles[id][misY] = y;
	gMissiles[id][misZ] = z;
	gMissiles[id][misVX] = vx;
	gMissiles[id][misVY] = vy;
	gMissiles[id][misVZ] = vz;

	SetObjectRot(gMissiles[id][misObjectID], cmiss_RetRXFromVectorZ(vz), 0.0, cmiss_RetRZFromVectorXY(vx, vy));
	MoveObject(gMissiles[id][misObjectID], x, y, z, speed);

	return 1;
}

// ---------------------------------------------------------- Internal

stock static Float:cmiss_RetRZFromVectorXY(Float:vx, Float:vy)
{
	if(vx == 0.0 && vy == 0.0) return 0.0;

	return atan2(vy, vx) - 90.0;
}

stock static Float:cmiss_RetRXFromVectorZ(Float:vz)
{
	return -(acos(vz) - 90.0);
}

stock static cmiss_NormalizeVector(&Float:x, &Float:y, &Float:z)
{
	new Float: len = VectorSize(x, y, z);

	if(len == 0.0) return 0;

	x = x / len;
	y = y / len;
	z = z / len;

	return 1;
}

stock static cmiss_randomEx(rmin, rmax)
{
	return (rmin + random(rmax - rmin));
}

stock static Float:cmiss_frandomEx(Float:min, Float:max = (Float:0x7FFFFFFF), dp = 3)
{
    new
        Float:mul = floatpower(10.0, float(dp));
    switch (dp)
    {
    case 0: mul = 1.0;
    case 1: mul = 10.0;
    case 2: mul = 100.0;
    case 3: mul = 1000.0;
    default: mul = floatpower(10.0, float(dp));
    }
    if (max != max)
    {
        if (min < 0.0)
            return -(float(random(floatround(-min * mul))) / mul);
        return float(random(floatround(min * mul))) / mul;
    }
    // Parameters are the wrong way around - do it anyway.
    if (max < min)
        return float(random(floatround(min * mul - max * mul))) / mul + max;
    // NOT a silly check - "IsNaN".
    return float(random(floatround(max * mul - min * mul))) / mul + min;
}

// ----------------------------------------------------------

// EOF
