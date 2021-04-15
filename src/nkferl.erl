-module(nkferl).

%% API exports
-export([nkf/2, guess/1]).
-on_load(init/0).

-define(APPNAME, nkferl).
-define(LIBNAME, nkferl).

%%====================================================================
%% API functions
%%====================================================================

nkf(_, _) ->
  not_loaded(?LINE).

guess(_) ->
  not_loaded(?LINE).

%%====================================================================
%% Internal functions
%%====================================================================

init() ->
  SoName = case code:priv_dir(?APPNAME) of
    {error, bad_name} ->
      case filelib:is_dir(filename:join(["..", priv])) of
        true ->
          filename:join(["..", priv, ?LIBNAME]);
        _ ->
          filename:join([priv, ?LIBNAME])
      end;
    Dir ->
        filename:join(Dir, ?LIBNAME)
  end,
  erlang:load_nif(SoName, 0).


not_loaded(Line) ->
  exit({not_loaded, [{module, ?MODULE}, {line, Line}]}).