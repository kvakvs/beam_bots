BEAM Bots Protocol version 0x00
===============================

BEAM Bots game is controlled via ZeroMQ RPC started (by default) on port 15000.
Commands are encoded in a binary format.

To play the game, user should:

1.  Create a game session and name his car (NEW_SESSION command).
2.  Place the car in the world (RESET command).
3.  Control the car by giving low level motor commands (CTL_MOTOR) or higher level
    drive and steer commands (CTL_FORWARD, CTL_STEER).
4.  To stop the car, send a CTL_FORWARD command with 0.0 as power or CTL_MOTOR
    command with 0.0 power on all wheels.

To read the sensors and world state, user can:

*   Run SEE_SELF command to know bot's location. Origin is located under the back
    bumper of the toy car (not the most brilliant location yes).
*   Run SEE_OBSTACLES command, result will come as array of distances to ray cast
    hits. NOTE: That player car silhouettes also come as obstacles.

Motor power value unit is revolutions per second.
Motor strength is hardcoded in the physical model.

For any command server may return 0x00 as first byte, followed by an error string.
When error is returned: Read 64-bit session id (or zero if no session is present),
then read 32-bit Uint32 length and UTF-8 bytes.

Data types
----------

*   Strings are encoded with 32 bit big-endian length, followed by UTF-8 characters
*   Float32 are encoded as IEEE 32-bit big-endian float.
*   Signed- and Unsigned Ints are encoded big-endian.

NEW_SESSION
-----------

This sets up a player session, no car is spawned. To spawn a car see RESET command.
Session will expire if no commands are sent to it in 60 seconds.

*   Client: Write byte 0x01 which marks a NEW_SESSION command.
*   Client: Write byte with protocol version, which must be 0x00.
*   Client: Write String player_name.

A reply will be sent to the caller with UInt64 session id. Use this id on
every subsequent call to control your robot.

Response:

*   Server: Write byte 0x01 which marks a response to NEW_SESSION
*   Server: Write UInt64 session id.

RESET
-----

Spawns a car on the playing field. If car already existed, it will be deleted.
Use this command to begin playing or to restart if your car is stuck or flipped over.

*   Client: Write byte 0x02 which marks a RESET command.
*   Client: Write UInt64 session id.

Response:

*   Server: Write byte RESET=0x02
*   Server: Write UInt64 session id.

CTL_MOTOR
---------

Low level motor control command which stops wheel motors.
Power values are applied to motors permanently until the next command.

*   Client: Write byte 0x03 which marks a CTL_MOTOR command.
*   Client: Write UInt64 session id.
*   Client: Write 4 Float32 with motor power for FrontLeft,
    FrontRight, BackLeft, and BackRight wheels.

Response:

*   Server: Write byte CTL_MOTOR=0x03
*   Server: Write UInt64 session id.

SEE_SELF
--------

Returns information about bot's current position and state.

*   Client: Write byte 0x04 which marks a SEE_SELF command.
*   Client: Write UInt64 session id.

Response:

*   Server: Write byte SEE_SELF=0x04
*   Server: Write UInt64 session id.
*   Server: Write 3 Float32 with location.
    X is up on screen, Y is right on screen, Z axis is vertical.
*   Server: Write 3 Float32 with orientation {X or Roll, Y or Pitch, Z or Yaw}.
    Any X or Y orientation value > ±90° means that the car has flipped over.

SEE_OBSTACLES
-------------

Casts a fan of rays in front of the vehicle. Returns distances to each hit.
Distance unit is centimeter. RAY_COUNT=16 rays are cast with a RAY_STEP=4° interval.

*   Client: Write byte 0x05 which marks a SEE_OBSTACLES command.
*   Client: Write UInt64 session id.

Response:

*   Server: Write byte SEE_OBSTACLES=0x05
*   Server: Write UInt64 session id.
*   Server: Write UInt32 = 16 (size of array)
*   Server: Write 16 Float32 with distances to the nearest collision with
    the geometry or another car. Special value 1'000'000.0 means "too far"
    (beyond the limit of RAY_MAX_DIST=3'000cm or 30m).

