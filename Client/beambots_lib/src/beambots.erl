-module(beambots).

%% ZeroMQ client 'chumak' must be started: application:start(chumak).
%% API exports
-export([
    connect/2,
    command_new_session/2,
    command_reset/1,
    command_control_motors/5
]).

-record(beambots, {
    socket,
    pid,
    session_id = undefined
}).

-define(CMD_NEW_SESSION, 1).
-define(CMD_RESET, 2).
-define(CMD_CTL_MOTOR, 3).

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
                (byte_size(NameBin)):8,
                NameBin/binary>>,
    zmq_send(BEAMBots, Message),
    %% Reply comes as ?CMD_NEW_SESSION + Uint64 session_id
    <<?CMD_NEW_SESSION:8, NewSession:64/big>> = zmq_recv(BEAMBots),
    BEAMBots#beambots{session_id = NewSession}.

-spec command_reset(#beambots{}) -> #beambots{}.
command_reset(#beambots{session_id=Sid} = BEAMBots) ->
    Message = <<?CMD_RESET:8, Sid:64/big>>,
    zmq_send(BEAMBots, Message),
    %% No reply to expect
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
    zmq:send(BEAMBots, Message),
    %% No reply to expect
    BEAMBots.

%%====================================================================
%% Internal functions
%%====================================================================
zmq_send(#beambots{socket=S}, Binary) ->
    case chumak:send(S, Binary) of
        ok -> ok;
        {error, E} -> erlang:error({beambots, zmq_send, E})
    end.

-spec zmq_recv(#beambots{}) -> binary().
zmq_recv(#beambots{socket=S}) ->
    case chumak:recv(S) of
        {ok, Message} ->
            Message;
        {error, E} ->
            erlang:error({beambots, zmq_recv, E})
    end.
