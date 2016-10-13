%%%-------------------------------------------------------------------
%% @doc bottest public API
%% @end
%%%-------------------------------------------------------------------

-module(bottest_app).

-behaviour(application).

%% Application callbacks
-export([start/2, stop/1]).

%%====================================================================
%% API
%%====================================================================

start(_StartType, _StartArgs) ->
    application:start(chumak),
    B0 = beambots:connect("localhost", 15000),
    io:format("B0 (before new session) ~p~n", [B0]),
    B1 = beambots:command_new_session(B0, "Erlang1"),
    io:format("B1 (after new session) ~p~n", [B1]),
    B2 = beambots:command_reset(B1),
    io:format("B2 (after reset) ~p~n", [B2]),
    B3 = beambots:command_control_motors(B2, 1.0, 1.0, 1.0, 1.0),
    {B4, Loc, Rot} = beambots:command_see_self(B3),
    io:format("SEE_SELF Location ~p~n         Rotation ~p~n", [Loc, Rot]),
    keep_scanning(1000, B4),
    init:stop().
    %bottest_sup:start_link().

keep_scanning(0, _) -> ok;
keep_scanning(Count, BEAMBots) ->
    {BEAMBots1, Dist} = beambots:command_see_obstacles(BEAMBots),
    %% Replace >= 1'000'000 with shorter atom 'far'
    Dist1 = lists:map(fun(X) when X >= 1000000.0 -> 'far'; (Y) -> Y end, Dist),
    io:format("SEE_OBSTACLES ~p~n", [Dist1]),
    timer:sleep(500),
    keep_scanning(Count-1, BEAMBots1).

%%--------------------------------------------------------------------
stop(_State) ->
    ok.

%%====================================================================
%% Internal functions
%%====================================================================
