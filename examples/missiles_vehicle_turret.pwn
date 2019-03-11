#define FILTERSCRIPT

#include <a_samp>
#include <ColAndreas>
#include <streamer>
#include <CAMissiles>
#include <zcmd>

/*

Test Script that attaches a rotatable Turret to the vehicle.

Type /turret to activate the Turret if in a vehicle.

Use KEY_FIRE (LMB) to shoot, submission (2) to switch Missile Mode.

For Homing Missile use KEY_ALT_FIRE/KEY_ACTION (RMB) to aim.
Any other player in range will be the target, otherwise a position is indicated by a Race Checkpoint.

You can rotate the Turret using NUM Pad 4/6, 2/8

Note: Missile Launching vector is not technically correct, it doesn't consider the vehicle rotation.
It's easy to do though if you want to use this or something similar (rotations.inc ftw).

*/

#define TURRET_CENTER_X_ROT		20.0

#define TURRET_MAX_X_ROT		34.0
#define TURRET_MAX_Z_ROT		180.0

#define TURRET_X_ROT_SPEED		2.0
#define TURRET_Z_ROT_SPEED		5.0

new TurretBase[MAX_PLAYERS][3], Turret[MAX_PLAYERS], TurretMode[MAX_PLAYERS], bool:TurretTarget[MAX_PLAYERS], Float:TurretTargetPos[MAX_PLAYERS][3], TurretVeh[MAX_PLAYERS], Float:TurretOffset[MAX_MISSILES][3], Float:TurretRotX[MAX_PLAYERS], Float:TurretRotZ[MAX_PLAYERS];

public OnFilterScriptInit()
{
	CA_Init();
	for(new i = 0; i < MAX_PLAYERS; i ++)
	{
		for(new j = 0; j < 3; j ++) TurretBase[i][j] = -1;

	    Turret[i] = -1;
	    TurretVeh[i] = -1;
	}

	SetTimer("KeyTimer", 70, 1);

	return 1;
}

public OnFilterScriptExit()
{
	for(new i = 0; i < MAX_PLAYERS; i ++)
	{
	    if(Turret[i] != -1)
	    {
	    	DestroyObject(Turret[i]);

	    	for(new j = 0; j < 3; j ++) DestroyObject(TurretBase[i][j]);

	    	Turret[i] = -1;
	    }
	}
	
	return 1;
}

public OnPlayerCommandText(playerid, cmdtext[])
{
	return 0;
}

public OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
    if(GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
    {
        new vid = GetPlayerVehicleID(playerid);
        
        if(Turret[playerid] == -1 || TurretVeh[playerid] != vid) return 1;

	    if(newkeys & KEY_SUBMISSION)
	    {
	    	TurretMode[playerid] ++;
	    	if(TurretMode[playerid] > MISSILE_MODE_GUARDED) TurretMode[playerid] = MISSILE_MODE_ARTILLERY;

	    	if(TurretTarget[playerid])
	    	{
	    		DisablePlayerRaceCheckpoint(playerid);
	    		TurretTarget[playerid] = false;
	    	}

	    	new text[60];
	    	switch(TurretMode[playerid])
	    	{
	    		case MISSILE_MODE_ARTILLERY: format(text, sizeof(text), "Turret Mode: {FF9999}Artillery");
	    		case MISSILE_MODE_RPG: format(text, sizeof(text), "Turret Mode: {FF9999}RPG");
	    		case MISSILE_MODE_HOMING: format(text, sizeof(text), "Turret Mode: {FF9999}Homing Missile - Hold RMB while stationary");
	    		case MISSILE_MODE_GUARDED: format(text, sizeof(text), "Turret Mode: {FF9999}Guarded Missile");
	    	}

	    	SendClientMessage(playerid, 0xFFFFFFFF, text);
	    }
        
		if(newkeys & KEY_FIRE && !(oldkeys & KEY_FIRE))
		{
			if(TurretMode[playerid] == MISSILE_MODE_HOMING && !TurretTarget[playerid]) return 1;

		    new Float:x, Float:y, Float:z, Float:sx, Float:sy, Float:sz, Float:a, Float:vx, Float:vy, Float:vz;

		    GetVehiclePos(vid, sx, sy, sz);
		    GetVehicleZAngle(vid, a);
		    a = -a - TurretRotZ[playerid];

		    sx += TurretOffset[playerid][1] * floatsin(a, degrees);
		    sy += TurretOffset[playerid][1] * floatcos(a, degrees);
		    sz += TurretOffset[playerid][2] + 0.13;

		    x = sx;
		    y = sy;
		    z = sz;

		    vx = 2.5 * floatsin(a, degrees);
		    vy = 2.5 * floatcos(a, degrees);
		    vz = 2.5 * floattan(TurretRotX[playerid] + TURRET_CENTER_X_ROT, degrees);

			switch(TurretMode[playerid])
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
					new targetplayer = -1;
					for(new i = 0; i < MAX_PLAYERS; i ++) if(i != playerid && IsPlayerConnected(i) && IsPlayerInRangeOfPoint(i, 150.0, x, y, z))
					{
						targetplayer = i;
						break;
					}

					if(targetplayer == -1) FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_HOMING, .target_type = MISSILE_TARGET_POS, .target_x = TurretTargetPos[playerid][0], .target_y = TurretTargetPos[playerid][1], .target_z = TurretTargetPos[playerid][2]);
					else FireMissile(x, y, z, vx, vy, vz, MISSILE_MODE_HOMING, .target_type = MISSILE_TARGET_PLAYER, .target_id = targetplayer);
				}
			}
		}
	}
	return 1;
}

forward KeyTimer();
public KeyTimer()
{
	new Float:x, Float:y, Float:z, Float:vx, Float:vy, Float:vz;

	for(new playerid = 0; playerid < MAX_PLAYERS; playerid ++)
	{
	    if(!IsPlayerConnected(playerid) || GetPlayerState(playerid) != PLAYER_STATE_DRIVER || Turret[playerid] == -1) continue;
	    
	    new vid = GetPlayerVehicleID(playerid);
	    
	    if(TurretVeh[playerid] != vid) continue;
	    
	    new keys, ud, lr;
	    
		GetPlayerKeys(playerid, keys, ud, lr);
		
		if(keys & KEY_ANALOG_DOWN && TurretRotX[playerid] <= TURRET_MAX_X_ROT)
		{
			TurretRotX[playerid] += TURRET_X_ROT_SPEED;

			if(TurretRotX[playerid] > TURRET_MAX_X_ROT) TurretRotX[playerid] = TURRET_MAX_X_ROT + 0.01;

			AttachObjectToVehicle(Turret[playerid], vid, 0.0, 0.0, TurretOffset[playerid][2], 0.0, -TurretRotX[playerid] - TURRET_CENTER_X_ROT, TurretRotZ[playerid] + 90.0);
		}
		else if(keys & KEY_ANALOG_UP && TurretRotX[playerid] >= -TURRET_MAX_X_ROT)
		{
		    TurretRotX[playerid] -= TURRET_X_ROT_SPEED;

			if(TurretRotX[playerid] < -TURRET_MAX_X_ROT) TurretRotX[playerid] = -TURRET_MAX_X_ROT - 0.01;
			
			AttachObjectToVehicle(Turret[playerid], vid, 0.0, 0.0, TurretOffset[playerid][2], 0.0, -TurretRotX[playerid] - TURRET_CENTER_X_ROT, TurretRotZ[playerid] + 90.0);
		}
		if(keys & KEY_ANALOG_LEFT)
		{
			TurretRotZ[playerid] += TURRET_Z_ROT_SPEED;

			if(TurretRotZ[playerid] > TURRET_MAX_Z_ROT) TurretRotZ[playerid] = TURRET_MAX_Z_ROT + 0.01;

			if(TurretRotZ[playerid] >= 180.0) TurretRotZ[playerid] -= 360.0;

			AttachObjectToVehicle(Turret[playerid], vid, 0.0, 0.0, TurretOffset[playerid][2], 0.0, -TurretRotX[playerid] - TURRET_CENTER_X_ROT, TurretRotZ[playerid] + 90.0);
		}
		else if(keys & KEY_ANALOG_RIGHT)
		{
		    TurretRotZ[playerid] -= TURRET_Z_ROT_SPEED;

			if(TurretRotZ[playerid] < -TURRET_MAX_Z_ROT) TurretRotZ[playerid] = -TURRET_MAX_Z_ROT - 0.01;

			if(TurretRotZ[playerid] < -180.0) TurretRotZ[playerid] += 360.0;
			
			AttachObjectToVehicle(Turret[playerid], vid, 0.0, 0.0, TurretOffset[playerid][2], 0.0, -TurretRotX[playerid] - TURRET_CENTER_X_ROT, TurretRotZ[playerid] + 90.0);
		}

		if(TurretMode[playerid] == MISSILE_MODE_HOMING)
		{
			GetVehicleVelocity(vid, x, y, z);

			if(/*VectorSize(x, y, z) < 0.05 && */keys & KEY_ACTION)
			{
				GetPlayerCameraPos(playerid, x, y, z);
				GetPlayerCameraFrontVector(playerid, vx, vy, vz);

				z += 5.0;

				if(!(0 < CA_RayCastLine(x, y, z, x + 200.0 * vx, y + 200.0 * vy, z + 200.0 * vz, x, y, z) < WATER_OBJECT))
				{
					x = x + 200.0 * vx;
					y = y + 200.0 * vy;
					z = z + 200.0 * vz;
				}
				
				SetPlayerRaceCheckpoint(playerid, 3, x, y, z, vx, vy, vz, 5.0);

				TurretTarget[playerid] = true;
				TurretTargetPos[playerid][0] = x;
				TurretTargetPos[playerid][1] = y;
				TurretTargetPos[playerid][2] = z;
			}
			else if(TurretTarget[playerid])
			{
				DisablePlayerRaceCheckpoint(playerid);
				TurretTarget[playerid] = false;
			}
		}
		else if(TurretTarget[playerid])
		{
			DisablePlayerRaceCheckpoint(playerid);
			TurretTarget[playerid] = false;
		}
	}
	return 1;
}

CMD:turret(playerid, const params[])
{
    if(GetPlayerState(playerid) != PLAYER_STATE_DRIVER) return 1;
    
    if(Turret[playerid] != -1)
    {
    	DestroyObject(Turret[playerid]);
    	for(new j = 0; j < 3; j ++) DestroyObject(TurretBase[playerid][j]);
    }
    
    new Float:x, Float:y, Float:z, vid = GetPlayerVehicleID(playerid); // Offsets
    GetVehicleModelInfo(GetVehicleModel(vid), VEHICLE_MODEL_INFO_SIZE, x, y, z);

    SetVehicleHealth(vid, 500000.0);
    
    TurretBase[playerid][0] = CreateObject(19843, 1392.217529, -2502.055175, 42.376834, 0.000000, 0.000000, 0.000000); // ID 0 (MetalPanel1)
	SetObjectMaterial(TurretBase[playerid][0], 0, 18250, "cw_junkbuildcs_t", "Was_scrpyd_rustmetal", 0);

	TurretBase[playerid][1] = CreateObject(19845, 1392.217529, -2502.025878, 42.921947, 90.000000, 0.000000, 0.000000); // ID 1 (MetalPanel3)
	SetObjectMaterial(TurretBase[playerid][1], 0, 10350, "oc_flats_gnd_sfs", "ws_screenedfence_big", 0);

	TurretBase[playerid][2] = CreateObject(19845, 1392.190673, -2502.055175, 42.921947, 90.000000, 0.000000, 90.000000); // ID 3 (MetalPanel3)
	SetObjectMaterial(TurretBase[playerid][2], 0, 10350, "oc_flats_gnd_sfs", "ws_screenedfence_big", 0);

	AttachObjectToVehicle(TurretBase[playerid][0], vid, 0.0, 0.0, z/2.0 - 0.05, 0.0, 0.0, 0.0);
	AttachObjectToVehicle(TurretBase[playerid][1], vid, 0.0, 0.0, z/2.0 - 0.05 + 0.545, 90.0, 0.0, 0.0);
	AttachObjectToVehicle(TurretBase[playerid][2], vid, 0.0, 0.0, z/2.0 - 0.05 + 0.545, 90.0, 0.0, 90.0);

    Turret[playerid] = CreateObject(359, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 900.0);

    AttachObjectToVehicle(Turret[playerid], vid, 0.0, 0.0, z/2.0 - 0.05 + 0.817, 0.0, -TURRET_CENTER_X_ROT, 90.0);

    TurretVeh[playerid] = vid;
    TurretRotX[playerid] = 0.0;
    TurretRotZ[playerid] = 0.0;
    
    TurretOffset[playerid][0] = 0.0;
    TurretOffset[playerid][1] = 0.0;
    TurretOffset[playerid][2] = z/2.0 - 0.05 + 0.817;

    TurretMode[playerid] = MISSILE_MODE_RPG;
    TurretTarget[playerid] = false;
    
    return 1;
}