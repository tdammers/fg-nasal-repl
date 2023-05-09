fg-nasal-repl
=============

A simple REPL for Nasal.

Installation
------------

Requirements:

- GNU Make
- GCC
- pkg-config
- The readline library & headers

To build, simply run `make`. This should produce a self-contained binary,
`nasali`.

You may move this binary to somewhere on your `$PATH` for convenience, but this
is not strictly required.

Usage
-----

Start FlightGear with the following options:

    --telnet=$PORT --allow-nasal-from-sockets

Then launch `nasali`:

    nasali $HOSTNAME $PORT

This opens an interactive session connected to your FlightGear instance.

$HOSTNAME should normally be `localhost`, but you can connect to a remote
FlightGear instance too.

$PORT defaults to 5501, and must match the port you passed to the `--telnet`
option.

Bugs / Caveats
--------------

This took about half an hour to write, so I'm sure there are plenty of bugs.

Known issues / caveats:

- Some Nasal commands return `nil`, but on a telnet connection, this will not
  produce any output at all, causing `select()` to time out. A reasonably short
  timeout has been selected, but if FG is struggling to keep up the
  performance, this could cause other responses to go missing; however,
  increasing the timeout means that legit `nil` responses take longer to
  appear.
- Nasal errors are not sent over telnet, and will thus not appear in the REPL.
  It is recommended to launch FG from a terminal, and keep that terminal
  visible next to the REPL, because that's where Nasal errors will be printed.
