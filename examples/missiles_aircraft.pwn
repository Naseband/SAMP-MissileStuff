#if 0
#pragma option -r
#pragma option -d3
#endif

#define FILTERSCRIPT

/*

Test Script for CAMissilePods

/cpod - spawns a weaponized Beagle

Controls for firing Missiles:
	
	Fire Key 	RPG (low delay)
	Alt Fire 	Swarm (medium delay)
	Submission 	Drop Bomb (artillery missile, long delay)

Some other offsets for vehicles if you want to add mor (x, y, z):

Rustler (Model 3790):
	left:	5.8, 0.6, 0.3
	right:	-5.8, 0.6, 0.3

(Sea) Sparrow (Model 3790):
	right:	0.5, 0.7, -0.3
	left:	-0.5, 0.7, -0.3 (the MG of the Sea Sparrow is in this position, so only for the regular one)

*/

#include <a_samp>
#define _FOREACH_NO_TEST
#include <foreach>
#include <ColAndreas>
#include <streamer>
#include <rotations>
#include <CAMissiles>
#include <CAMissilePods>

#include <zcmd>
#include <sscanf2>


new TestVeh = 0, MPod_RPG = -1, MPod_Swarm = -1, MPod_Artillery = -1;

public OnFilterScriptInit()
{
	CA_Init();

	for(new i = 0; i < MAX_PLAYERS; i ++) if(IsPlayerConnected(i))
	{
		OnPlayerConnect(i);
	}

	return 1;
}

public OnFilterScriptExit()
{
	DestroyVehicle(TestVeh);
	DestroyAllMissilePods();

	TestVeh = 0;
	MPod_RPG = -1;
	MPod_Swarm = -1;
	MPod_Artillery = -1;

	return 1;
}

public OnPlayerConnect(playerid)
{
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	return 0;
}

CMD:cpod(playerid, const params[])
{
	if(TestVeh) DestroyVehicle(TestVeh);

	DestroyMissilePod(MPod_RPG);
	DestroyMissilePod(MPod_Swarm);
	DestroyMissilePod(MPod_Artillery);

	new Float:x, Float:y, Float:z;
	GetPlayerPos(playerid, x, y, z);

	TestVeh = CreateVehicle(511, x, y + 7.7, z + 6.0, 0.0, 126, 126, 5, 1);

	MPod_RPG = CreateMissilePod(3790, 800, 1000, KEY_FIRE, MISSILE_MODE_RPG, 5000, 100.0);
	MPod_Swarm = CreateMissilePod(3786, 1500, 1000, KEY_ACTION, MISSILE_MODE_SWARM, 6000, 70.0);
	MPod_Artillery = CreateMissilePod(1636, 1000, 1000, KEY_SUBMISSION, MISSILE_MODE_ARTILLERY, 6000, 40.0);

	AttachMissilePodToVehicle(MPod_RPG, TestVeh, -6.4, -0.8, 0.22, 0.0, 0.0, 270.0);
	AttachMissilePodToVehicle(MPod_Swarm, TestVeh, 6.4, -0.8, 0.125, 0.0, 0.0, 270.0);
	AttachMissilePodToVehicle(MPod_Artillery, TestVeh, 0.0, -0.8, -1.65, 0.0, 0.0, 0.0);

	return 1;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	
	// Some old test code, there are some offsets for a Rustler with Missiles and a regular Sparrow with Missiles

	/*new vid;

	if((vid = GetPlayerVehicleID(playerid)) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
	{
		if(vid)
		{
			switch(GetVehicleModel(vid))
			{
				case 511: // Beagle - Missiles and Bombs
				{
					if(newkeys & KEY_FIRE && !(oldkeys & KEY_FIRE))
					{
						new Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:vx, Float:vy, Float:vz, Float:matrix[4][4];

						//GetVehiclePos(vid, x, y, z);
						//GetVehicleRotationQuat(vid, rw, rx, ry, rz);

						if(gVehiclePodState{vid - 1})
						{
							GetVehicleObjectPositionWorld(vid, -5.8, 0.6, 0.3, 0.0, 0.0, 0.0, x, y, z, rx, ry, rz);
							GetRotationMatrixFromEuler(matrix, rx, ry, rz, euler_samp);
							MatrixRotate(matrix, 0.0, 1.0, 0.0, 0.0, vx, vy, vz);

							FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_RPG, 7000, 90.0, 1.0, 0.0);

							gVehiclePodState{vid - 1} = 0;
						}
						else
						{
							GetVehicleObjectPositionWorld(vid, , 0.0, 0.0, 0.0, x, y, z, rx, ry, rz);
							GetRotationMatrixFromEuler(matrix, rx, ry, rz, euler_samp);
							MatrixRotate(matrix, 0.0, 1.0, 0.0, 0.0, vx, vy, vz);

							FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_RPG, 7000, 90.0, 1.0, 0.0);

							gVehiclePodState{vid - 1} = 1;
						}
					}
				}

				case 476: // Rustler - Missiles and MGs
				{
					if(newkeys & KEY_FIRE && !(oldkeys & KEY_FIRE))
					{
						new Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:vx, Float:vy, Float:vz, Float:matrix[4][4];

						if(gVehiclePodState{vid - 1})
						{
							GetVehicleObjectPositionWorld(vid, -2.5, 0.7, -0.5, 0.0, 0.0, 0.0, x, y, z, rx, ry, rz);
							GetRotationMatrixFromEuler(matrix, rx, ry, rz, euler_samp);
							MatrixRotate(matrix, 0.0, 1.0, 0.0, 0.0, vx, vy, vz);

							FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_RPG, 4500, 120.0, 1.0, 0.0);

							gVehiclePodState{vid - 1} = 0;
						}
						else
						{
							GetVehicleObjectPositionWorld(vid, 2.5, 0.7, -0.5, 0.0, 0.0, 0.0, x, y, z, rx, ry, rz);
							GetRotationMatrixFromEuler(matrix, rx, ry, rz, euler_samp);
							MatrixRotate(matrix, 0.0, 1.0, 0.0, 0.0, vx, vy, vz);

							FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_RPG, 4500, 120.0, 1.0, 0.0);

							gVehiclePodState{vid - 1} = 1;
						}
					}
				}

				case 469: // Sparrow - Missiles
				{
					if(newkeys & KEY_FIRE && !(oldkeys & KEY_FIRE))
					{
						new Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:vx, Float:vy, Float:vz, Float:matrix[4][4];

						if(gVehiclePodState{vid - 1})
						{
							GetVehicleObjectPositionWorld(vid, 0.5, 0.7, -0.3, 0.0, 0.0, 0.0, x, y, z, rx, ry, rz);
							GetRotationMatrixFromEuler(matrix, rx, ry, rz, euler_samp);
							MatrixRotate(matrix, 0.0, 1.0, 0.0, 0.0, vx, vy, vz);

							FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_RPG, 6000, 65.0, 1.0, 0.01);

							gVehiclePodState{vid - 1} = 0;
						}
						else
						{
							GetVehicleObjectPositionWorld(vid, -0.5, 0.7, -0.3, 0.0, 0.0, 0.0, x, y, z, rx, ry, rz);
							GetRotationMatrixFromEuler(matrix, rx, ry, rz, euler_samp);
							MatrixRotate(matrix, 0.0, 1.0, 0.0, 0.0, vx, vy, vz);

							FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_RPG, 6000, 65.0, 1.0, 0.01);

							gVehiclePodState{vid - 1} = 1;
						}
					}
				}
			}
		}
	}*/

	return 1;
}

public OnPlayerWeaponShot(playerid, weaponid, hittype, hitid, Float:fX, Float:fY, Float:fZ)
{
	printf("dsds");

	return 1;
}
