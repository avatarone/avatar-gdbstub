avatar-gdbstub
==============

GDB stub that allows debugging of embedded devices


This is the source code for a very small GDB stub implementation.
The stub is supposed to communicate with the host GDB over the
GDB serial protocol or a custom protocol (Avatar).

Depending on how the serial line is used, you need a Multiplexer
to set up the environment for you and send any stuff that is needed
until the GDB stub is running.

Adapting the GDB stub for your platform is easy: Copy one of the
existing gdbstub_*.cmake files in the cmake/ directory and adapt
it to your platform. You might need to develop additional serial
drivers and platform initialization files.
