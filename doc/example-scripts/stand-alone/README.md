## bplib Example Scripts - Stand-alone bplib

Define `BPLIB_HOME` in `.bashrc` with `export BPLIB_HOME=~/bplib-home`
`BPLIB_HOME` can be any folder for which the user has rwx permissions.

Copy the example scripts to $BPLIB_HOME.

Verify that `BPLIB_SOURCE` in `bplib-env-vars` is the path to a clone of the bplib repository.

Run the scripts in the following order:

```sh
cd $BPLIB_HOME
source bplib-env-vars
# Configuration
./bplib-install-toolchain
# Test
./bplib-test-driver  # Runs bplib-unit-test-functional
```

To remove the test artifacts run `./bplib-clean`.

`bplib-clean` prompts to confirm the `rm` command.

