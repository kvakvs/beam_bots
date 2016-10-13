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
    io:format("B1 (after reset) ~p~n", [B2]),
    init:stop().
    %bottest_sup:start_link().

%%--------------------------------------------------------------------
stop(_State) ->
    ok.

%%====================================================================
%% Internal functions
%%====================================================================
