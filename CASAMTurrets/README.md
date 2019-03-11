# CASAMTurrets Include (c) 2017-2019 NaS

This include adds SAM Turrets for CAMissiles.
SAM Turrets will shoot hostile players in range with homing missiles.

There are two types of SAM Turrets:

- SAM_TYPE_AIR 		Shoots all airplanes and helis in range
- SAM_TYPE_GROUND 	Shoots all ground vehicles in range 	

# Functions:

- CreateSAMTurret(type, modelid, interval, Float:x, Float:y, Float:z, Float:rx, Float:ry, Float:rz, Float:min_z, Float:max_z, Float:range = 100.0, Float:speed = 30.0, Float:skill = 0.4, bool:can_rotate = true, Float:rot_speed = SAM_TURRET_ROT_SPEED, virtual_world = 0, interior = 0)
	
	Creates a SAM Turret.
	
	Returns the SAM ID if successful, otherwise -1.
	
	Note: modelid must be predefined in the include, otherwise the missile will spawn at the object's origin.

- IsValidSAMTurret(id)

	Returns 1 if valid, 0 otherwise.

- DestroySAMTurret(id)

	Destroys a SAM Turret.

- DestroyAllSAMTurrets()

	Destroys all SAM Turrets.

- ToggleSAMTurret(id, toggle)

	Toggles a SAM Turret on/off.

- GetSAMTurretBarrelPos(modelid, Float:base_x, Float:base_y, Float:base_z, Float:base_rx, Float:base_rz, &Float:x, &Float:y, &Float:z, &Float:vx, &Float:vy, &Float:vz)

	Gets the Missile Spawn Position and Vector for a model and position/rotation.

- SetSAMTurretCustomArea(id, areaid)

	Destroys the original Area and sets it to the specified one. Useful if you want to use a Polygon or other shape for the Turrets.
	Note: The range specified in CreateSAMTurret still applies (2D).

- Float:RotateSAMTurret(id, Float:rz)

	Rotates a SAM Turret to the specified angle.
	
	Returns the current angle difference (0.0 if already rotated).

- Float:RotateSAMTurretTo(id, Float:x, Float:y, Float:z)

	Rotates a SAM Turret towards a point.
	
	Returns the current angle difference (0.0 if already rotated).

- FireSAMTurret(id, targetid)

	Fires a Missile from specified Turret.
	
	targetid's type depends on the SAM Turret Type, currently only occupied vehicles can be tracked.
	
	Note: ignores target distance, rotation and firerate.

- FindSAMTurretTarget(id)

	Scans for a new target and updates the Turret's State if found.
	
	Returns 1 on success, 0 otherwise.

- SetSAMTurretHostileState(id, playerid, hostile_state)

	Set a Turret's Hostile State towards a player.
	
	Use playerid = -1 to apply for all players.

- GetSAMTurretHostileState(id, playerid)

	Gets a Turret's Hostile State towards a player.
	
	Returns 1 if hostile, 0 otherwise.
