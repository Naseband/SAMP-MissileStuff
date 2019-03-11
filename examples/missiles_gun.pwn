#define FILTERSCRIPT

/*

Test Script to fire Missiles by Shooting.

/gmissiles [type]

Types:
		0 - Artillery
		1 - RPG
		2 - Homing (follows hit target)
		4 - Swarm
		
*/

#include <a_samp>
#include <ColAndreas>
#include <streamer>
#include <CAMissiles>
#include <zcmd>
#include <sscanf2>

new gMissileMode[MAX_PLAYERS];

public OnFilterScriptInit()
{
	for(new i = 0; i < MAX_PLAYERS; i ++) gMissileMode[i] = -1;
	CA_Init();

	return 1;
}

public OnFilterScriptExit()
{
	return 1;
}

public OnPlayerConnect(playerid)
{
	gMissileMode[playerid] = -1;

	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	return 0;
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	if(gMissileMode[playerid] != -1)
	{
		GivePlayerWeapon(playerid, weaponid, 1);

		new Float:x, Float:y, Float:z, Float:hx, Float:hy, Float:hz, Float:vx, Float:vy, Float:vz;
		GetPlayerLastShotVectors(playerid, x, y, z, hx, hy, hz);

		if(VectorSize(x, y, z) < 0.1 || VectorSize(hx, hy, hz) < 0.1) return 1;

		vx = hx - x;
		vy = hy - y;
		vz = hz - z;

		new Float:len = VectorSize(vx, vy, vz);

		if(len == 0.0) return 1;

		/*new Float:dist = VectorSize(hx - ox, hy - oy, hz - oz);

		if(dist > 50.0) return 1;

		GetPlayerPos(playerid, fX, fY, fZ);

		if(VectorSize(fX - hx, fY - hy, fZ - hz) > 50.0) return 1;*/

		switch(gMissileMode[playerid])
		{
			case MISSILE_MODE_ARTILLERY:
			{
				FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_ARTILLERY);	
			}
			case MISSILE_MODE_RPG:
			{
				FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_RPG);
			}
			case MISSILE_MODE_HOMING:
			{
				switch(hittype)
				{
					case BULLET_HIT_TYPE_PLAYER:
					{
						FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_HOMING, .target_type = MISSILE_TARGET_PLAYER, .target_id = hitid);
					}
					case BULLET_HIT_TYPE_VEHICLE:
					{
						FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_HOMING, .target_type = MISSILE_TARGET_VEHICLE, .target_id = hitid);
					}
					default:
					{
						FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_HOMING, .target_type = MISSILE_TARGET_POS, .target_x = hx, .target_y = hy, .target_z = hz);
					}
				}
			}
			case MISSILE_MODE_GUARDED:
			{
				// TBD
			}
			case MISSILE_MODE_SWARM:
			{
				FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_SWARM, 25000, MISSILE_SPEED, 0.5, 0.0);
			}
		}
	}

	return 1;
}

CMD:gmissile(playerid, const params[])
{
	new mode;
	if(!sscanf(params, "i", mode)) gMissileMode[playerid] = mode;

	return 1;
}
