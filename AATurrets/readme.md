Player controlled AA Turrets by NaS (c) 2021

Turrets are created via the CreateAATurret function. From there, you need to manage players getting in/out
 using PutPlayerInAATurret and RemovePlayerFromAATurret or ClearAATurret.

The turrets have a limited rotation speed, while looking around it will rotate where the camera is aiming.
When using the fire key (KEY_FIRE) the script will call OnAATurretFire at the turret's fire rate repeatedly.

The default values can be redefined before including this include, but you can also set most of them per-turret.

In the callback you must spawn a projectile yourself. My CAMissile include will work fine against
 players and NPCs (including FCNPC), see the example. It is also capable of firing homing missiles.
If you prefer rotational angles instead of a vector, simply use GetAATurretCurrentRotation inside the callback.

Also it is not strictly against aircraft. You can fire downwards/horizontally too, if the max. rotation values allow it.

Originally made for my Contact Missions gamemode (base defender mission line).
