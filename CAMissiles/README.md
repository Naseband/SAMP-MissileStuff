# CAMissiles Include (c) 2017-2019 NaS

This include adds Missiles that can be fired by the script.

There are various modes which can be chosen:

- MISSILE_MODE_ARTILLERY

Fires a gravity-affected projectile that explodes on impact.

- MISSILE_MODE_RPG

RPG Missiles travel in a straight line until they collide.

- MISSILE_MODE_HOMING

Homing Missiles follow a target until they collide with the world or get close enough. Targets can be Positions, (Dyn.-/Pl.-)Objects, Players and Vehicles. Scripts can update the target position manually which (for example) could be used for a HL2-like guarded missile. For AA Missiles a speed for 90.0 or greater is recommended, unless you want Players to be able to evade them with a fast airplane. Homing Missiles will activate tracking 700 ms after launch to avoid an early collision.

- MISSILE_MODE_GUARDED (Not yet implemented)

Guarded Missiles are controlled by the Player that fires them. The player will enter missile view, directly controlling the missile using an attached camera.

- MISSILE_MODE_SWARM

Swarm Missiles detonate when in vicinity to a potential collision, launching multiple Child-Missiles in RPG Mode (extremely good for Air to Ground).

For now Missiles use global objects. This is mostly for cosmetic reasons, as streamer objects stutter if constantly changing directions when moving. This also uses less resources on the server-side. Keep that in mind when changing the MAX_MISSILES limit.

# Functions:

- FireMissile(Float:x, Float:y, Float:z, Float:vx, Float:vy, Float:vz, mode = MISSILE_MODE_RPG, ttl = MISSILE_TTL, Float:speed = MISSILE_SPEED, Float:step = MISSILE_STEP, Float:jitter = MISSILE_JITTER, Float:skill = MISSILE_SKILL, Float:down_force = MISSILE_DOWN_FORCE, target_type = -1, target_id = -1, Float:target_x = 0.0, Float:target_y = 0.0, Float:target_z = 0.0)
	
	Launches a missile.
	Returns Missile ID or -1 if invalid.
	
	mode - Missile Mode (see above)
	
	ttl - Time-to-Live in ms
	
	speed - Missile Speed in m/s

	step - Step Distance in m (1.5 or greater recommended for fast Missiles)

	jitter - Missile Jitter in m/step. Setting this to 0.01 will make the Missile "wobble" slightly. 0.1 or greater will make the Missile go all around the place.

	skill - Used for Homing Missiles and Guarded Missiles. 0.0 = tracking has no effect, 1.0 = instant turns. For Guarded Missiles this describes the minimum turning angle.

	down_force - Used for Artillery in m/step.

	target_id, target_x, etc. - Used for Homing Missiles. Depending on the target_type these have to be filled accordingly.

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
	
- ProcessMissile(id)

	Moves the Missile's Object to the next step. Does not need to be called at all unless the Missile was stopped.
