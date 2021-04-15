nkferl
=====

nkf library implemented in NIFs for Erlang.

Build
-----

    $ rebar3 compile

Usage
-----

encode the string:

```
1> {ok, Str} = nkferl:nkf("-s", <<"寒くなると、山の手大通りの露店に古着屋の数が殖える。">>).
{ok,<<106,139,104,1,113,110,75,39,26,138,110,50,151,107,
      228,64,75,110,112,76,150,72,0>>}
```

guess the encoding:

```
2> nkferl:guess(Str).
{ok,<<"Shift_JIS">>}
```
