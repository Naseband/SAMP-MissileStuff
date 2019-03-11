#CAMissiles Include (c) 2017-2019 NaS

This include adds Missiles that can be fired by the script.

There are various modes which can be chosen:
- Artillery 	Fires a gravity-affected projectile that explodes on impact
- RPG 			Fires an RPG projectile that travels in a straight line.
- Homing 		Fires a homing missile at a certain target.
				Targets can be Positions, (Dyn.-/Pl.-)Objects, Players and Vehicles.
				Scripts can update the target position manually which (for example) could
				be used for a HL2-like guarded missile.
- Guarded 		Fires a guarded missile. Must be fired by a player. The player will enter
				missile view, directly controlling the missile.
- Swarm 		Fires a swarm missile, it detonates when close to any collision, launching
				multiple RPGs in random directions (extremely good for Air to Ground)

#Functions:

- FireMissile(	Float:x, Float:y, Float:z, // Position
				Float:vx, Float:vy, Float:vz,  // Vector (does not need to be normalized)
				mode = MISSILE_MODE_RPG, // Missile Mode
				ttl = MISSILE_TTL, // Missile time-to-live (in ms)
				Float:speed = MISSILE_SPEED,  // Missile Speed
				Float:step = MISSILE_STEP, // Missile Step Size
				Float:jitter = MISSILE_JITTER, // Missile Jitter
				Float:skill = MISSILE_SKILL, // Missile Skill (0.0 - 1.0, 0.0 = RPG, 1.0 = instant turns) for HOMING
				Float:down_force = MISSILE_DOWN_FORCE, // Missile Gravity for ARTILLERY
				target_type = -1,
				target_id = -1,
				Float:target_x = 0.0, Float:target_y = 0.0, Float:target_z = 0.0)
	
	Launches a missile.
	Returns Missile ID or -1 if invalid.

- IsValidMissile(id)
	
	Returns 1 if the specified Missile ID is valid, 0 otherwise.

- IsValidMissileObject(id)

	Returns 1 if the specified Missile ID's associated Object is valid, 0 otherwise.

- ExplodeMissile(id)

	Detonates and destroys the specified Missile ID.
	Returns 1 on success, 0 otherwise.
	Note: Swarm missiles will fire its other Missiles when this is called.

- DestroyMissile(id)

	Destroys the specified Missile ID without detonating.