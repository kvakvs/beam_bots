# BEAM Bots

UnrealEngine4 (UE4) arena game with robots controlled via API.
Primarily intended as training exercise for students who learn programming,
or as a challenge between multiple bots (when more game modes are implemented).

# Building

Open with Unreal Editor on Mac or Windows.

# Running Client

See Erlang example (using rebar3) in the `Client/` directory.
Run it with `rebar3 shell` and then in Erlang console run `application:start(bottest).`

Example application `bottest` will connect to localhost, spawn 1 bot, print
its current location/orientation and start driving forward.
Every 0.25sec it will scan cone ahead and print the distances to obstacles.

See detailed protocol documentation in the `Doc/` directory.
