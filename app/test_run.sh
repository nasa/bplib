#!/bin/bash

# test script to launch locally built bpsend and bprecv executables in their own terminal windows

ROOT=$(pwd)

gnome-terminal --working-directory=$ROOT/build -- /bin/bash -c './bprecv ipn:5.1 udp://127.0.0.1:34405; exec /bin/bash'

sleep 1

gnome-terminal --working-directory=$ROOT/build -- /bin/bash -c './bpsend ipn:5.1 udp://127.0.0.1:34405; exec /bin/bash -i'

