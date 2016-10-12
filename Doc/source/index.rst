BEAM Bots Protocol 0.1
======================

BEAM Bots game is controlled via ZeroMQ RPC started (by default) on port 15000.
Commands are encoded in a binary format.

To play the game, user should:

1.  Create a game session and name his car (NEW_SESSION command).
2.  Place the car in the world (RESET command).
3.  Control the car by giving low level motor commands (CTL_MOTOR) or higher level
    drive and steer commands (CTL_FORWARD, CTL_STEER).
4.  To stop the car, send a CTL_FORWARD command with 0.0 as power or CTL_MOTOR
    command with 0.0 power on all wheels.

Motor power value unit is revolutions per second.
Motor strength is hardcoded in the physical model.

Data types
----------

*   ShortString are encoded with 8 bit length, followed by ASCII characters
*   BigEndianFloat32 are encoded as IEEE 32-bit big-endian float.
*   Signed- and UnsignedInts are encoded big-endian.

NEW_SESSION
-----------

This sets up a player session, no car is spawned. To spawn a car see RESET command.
Session will expire if no commands are sent to it in 60 seconds.

*   Client: Write byte 0x01 which marks a NEW_SESSION command.
*   Client: Write ShortString player name.

A reply will be sent to the caller with UnsignedInt64 session id. Use this id on
every subsequent call to control your robot.

*   Client: Read byte 0x01 which marks a response to NEW_SESSION
*   Client: Read UInt64 session id.

RESET
-----

Spawns a car on the playing field. If car already existed, it will be deleted.
Use this command to begin playing or to restart if your car is stuck or flipped over.

*   Client: Write byte 0x02 which marks a RESET command.
*   Client: Write U64 session id.

There is no server response to this command.

CTL_MOTOR
---------

Low level motor control command which stops wheel motors.
Power values are applied to motors permanently until the next command.

*   Client: Write byte 0x03 which marks a CTL_MOTOR command.
*   Client: Write U64 session id.
*   Client: Write 4 Float32 with motor power for FrontLeft,
    FrontRight, BackLeft, and BackRight wheels.

There is no server response to this command.

.. comment:
    CTL_FORWARD
    -----------
    Commands motors to spin, pushing the car forward.
    Has same effect as low level CTL_MOTOR with same value repeated 4 times.
    Positive values will push the car forward.
    *   Client: Write byte 0x80 which marks a CTL_FORWARD command.
    *   Client: Write U64 session id.
    *   Client: Write 1 Float32 with power, which will be applied to all 4 motors
        permanently until the next command.
    There is no server response to this command.
    CTL_STEER
    ---------
    Commands motors to spin in opposite directions, slowly rotating the car.
    Has same effect as low level CTL_MOTOR with opposite power values applied to
    the left and to the right sides of the car.
    Positive steer value will turn clockwise (right), negative steer will turn
    counter-clockwise (left).
    *   Client: Write byte 0x81 which marks a CTL_STEER command.
    *   Client: Write U64 session id.
    *   Client: Write 1 Float32 with power, which will be applied to motors
        permanently until the next command.
    There is no server response to this command.

