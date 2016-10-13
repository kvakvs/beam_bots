-module(beambots).

%% ZeroMQ client 'chumak' must be started: application:start(chumak).
%% API exports
-export([
    connect/2,
    command_new_session/2,
    command_reset/1,
    command_control_motors/5,
    command_see_self/1,
    command_see_obstacles/1
]).

-record(beambots, {
    socket,
    pid,
    session_id = undefined
}).

-define(PROTOCOL_VER,      0).
-define(CMD_ERROR,         0).
-define(CMD_NEW_SESSION,   1).
-define(CMD_RESET,         2).
-define(CMD_CTL_MOTOR,     3).
-define(CMD_SEE_SELF,      4).
-define(CMD_SEE_OBSTACLES, 5).

%%====================================================================
%% API functions
%%====================================================================
-spec connect(Host::string(), Port::pos_integer()) -> #beambots{}.

connect(Host, Port) ->
    {ok, Socket} = chumak:socket(req, "beambots-req"),
    case chumak:connect(Socket, tcp, Host, Port) of
        {ok, Pid} ->
            #beambots{socket=Socket, pid=Pid};
        {error, E} ->
            erlang:error({beambots, connect, E})
    end.

%% @doc Requests a new session with player name. No car is created at
%% this moment - use RESET command to create a car.
%% Returns: New #beambots{} value with session id updated.
-spec command_new_session(#beambots{}, Name::string()) -> #beambots{}.
command_new_session(#beambots{} = BEAMBots, Name) ->
    NameBin = list_to_binary(Name),
    Message = <<?CMD_NEW_SESSION:8,
                ?PROTOCOL_VER:8,
                (byte_size(NameBin)):32/big, NameBin/binary>>,

    zmq_send(BEAMBots, Message),

    %% Reply comes as ?CMD_NEW_SESSION + Uint64 session_id
    <<?CMD_NEW_SESSION:8, NewSession:64/big>> = zmq_recv(BEAMBots),
    BEAMBots#beambots{session_id = NewSession}.

-spec command_reset(#beambots{}) -> #beambots{}.
command_reset(#beambots{session_id=Sid} = BEAMBots) ->
    Message = <<?CMD_RESET:8, Sid:64/big>>,
    zmq_send(BEAMBots, Message),
    <<?CMD_RESET:8, Sid:64/big>> = zmq_recv(BEAMBots),
    BEAMBots.

%% @doc Sends power to toy car motors in this order:
%% Front left, Front right, Back left, Back right.
%% Power unit is revolutions per second. 0.0 will stop the motor.
-spec command_control_motors(#beambots{},
                             float(), float(), float(), float()) ->
    #beambots{}.
command_control_motors(#beambots{session_id=Sid} = BEAMBots, FL, FR, BL, BR) ->
    Message = <<?CMD_CTL_MOTOR:8,
                Sid:64/big,
                FL:32/float-big,
                FR:32/float-big,
                BL:32/float-big,
                BR:32/float-big>>,
    zmq_send(BEAMBots, Message),
    <<?CMD_CTL_MOTOR:8, Sid:64/big>> = zmq_recv(BEAMBots),
    BEAMBots.

%% @doc Returns current state of the robot.
%% At the moment available values are Location{x,y,z} and Rotation{x,y,z}
%% Z is vertical axis. X,Y rotation >= 90° means bot has flipped over.
-spec command_see_self(#beambots{}) ->
    {#beambots{}, {float(),float(),float()}, {float(),float(),float()}}.
command_see_self(#beambots{session_id=Sid} = BEAMBots) ->
    Message = <<?CMD_SEE_SELF:8, Sid:64/big>>,
    zmq_send(BEAMBots, Message),
    <<?CMD_SEE_SELF:8, Sid:64/big,
      LocX:32/float-big, LocY:32/float-big, LocZ:32/float-big,
        RotX:32/float-big, RotY:32/float-big, RotZ:32/float-big>> = zmq_recv(BEAMBots),
    Loc = {LocX, LocY, LocZ},
    Rot = {RotX, RotY, RotZ},
    {BEAMBots, Loc, Rot}.

%% @doc Traces a fan of rays in front of the bot.
%% (currently Count=16 rays with 3° step are cast)
%% Returns distances to nearest obstacle in each ray (other cars are also
%% considered as an obstacle) without specifying what type the obstacle was.
%% Unit of distance: 1 cm
-spec command_see_obstacles(#beambots{}) ->
    {#beambots{}, [float()]}.
command_see_obstacles(#beambots{session_id=Sid} = BEAMBots) ->
    Message = <<?CMD_SEE_OBSTACLES:8, Sid:64/big>>,
    zmq_send(BEAMBots, Message),
    <<?CMD_SEE_OBSTACLES:8, Sid:64/big, _Count:32/big, Data/binary>> = zmq_recv(BEAMBots),
    Result = [Dist || <<Dist:32/float>> <= Data],
    {BEAMBots, Result}.

%%====================================================================
%% Internal functions
%%====================================================================
zmq_send(#beambots{socket=S}, Message) ->
    case chumak:send(S, Message) of
        ok -> 
            io:format("beambots: Sent ~p bytes~n", [byte_size(Message)]),
            ok;
        {error, E} ->
            erlang:error({beambots, zmq_send, E})
    end.

-spec zmq_recv(#beambots{}) -> binary().
zmq_recv(#beambots{socket=S}) ->
    case chumak:recv(S) of
        {ok, <<?CMD_ERROR, _StringLength:32/big, Message1/binary>>} ->
            io:format("beambots: Received error ~s~n", [Message1]),
            erlang:error({beambots, 'ERROR', Message1});
        {ok, Message2} ->
            io:format("beambots: Received ~p bytes~n", [byte_size(Message2)]),
            Message2;
        {error, E} ->
            erlang:error({beambots, zmq_recv, E})
    end.
