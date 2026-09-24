# Local `tracy` recipe

ConanCenter's newest `tracy` is **0.13.1**, which speaks Tracy wire
**ProtocolVersion 76**. The `tracy` package installed on this machine supplies the
*server* side (`tracy-profiler`, `tracy-capture`) at **0.14.0**, protocol **82**.
Tracy hard-rejects a cross-protocol handshake:

    The client you are trying to connect to uses incompatible protocol version.

This recipe is ConanCenter's, unmodified apart from `conandata.yml` gaining the
0.14.0 tarball, so the client matches the installed profiler.

Export it into the local cache (once, or after editing):

    conan export conan/recipes/tracy --version 0.14.0

Drop this the moment ConanCenter ships 0.14.0: delete the directory and the
`conan export` line from `build.sh`.

Check for a protocol change after any tracy bump:

    grep ProtocolVersion /usr/include/Tracy/common/TracyProtocol.hpp   # server
