nkferl
=====

nkf library implemented in NIFs for Erlang.

(originated in vanished https://github.com/tayutayura/nkferl)

Build on Erlang
-----

    $ rebar3 compile

Install on Elixir
-----------------

```elixir
  defp deps do
    [
      {:nkferl, github: "ryochin/nkferl", branch: :main}
    ]
  end
```

Usage
-----

encode the string:

```erlang
1> {ok, Str} = nkferl:nkf("-s", <<"寒くなると、山の手大通りの露店に古着屋の数が殖える。">>).
{ok,<<106,139,104,1,113,110,75,39,26,138,110,50,151,107,
      228,64,75,110,112,76,150,72,0>>}
```

```elixir
iex> {:ok, str} = :nkferl.nkf('-s', "寒くなると、山の手大通りの露店に古着屋の数が殖える。")
{:ok, <<138, 166, 130, 173, 130, 200, 130, 233, ...>>}
```

guess the encoding:

```erlang
2> nkferl:guess(Str).
{ok,<<"Shift_JIS">>}
```

```elixir
iex> str |> :nkferl.guess
{:ok, "Shift_JIS"}
```