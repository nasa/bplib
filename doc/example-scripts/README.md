## bplib Example Scripts - cFS Build

Define `CFS_HOME` in `.bashrc` with `export CFS_HOME=~/cfs-home`
`CFS_HOME` can be any folder for which the user has rwx permissions.

Copy the example scripts to $CFS_HOME.

Verify that environment variables in $CFS_HOME are set to the correct paths.

The default paths are:

```
export CFS_HOME=~/cfs-home                # The build/test folder
export CFS_REPO=~/repos/cfs-bundle        # A clone of https://github.com/nasa/cFS
export BP_SOURCE=$CFS_REPO/apps/bp        # The bp folder within the cFS clone
export BPLIB_SOURCE=$CFS_REPO/libs/bplib  # The bplib folder within the cFS clone
```

Run the scripts in the following order:

Review `initialize-cfs-bundle` to verify that it will run without error. The script will report an error and exit if the `CFS_REPO` folder already exists and is not empty.

```sh
cd $CFS_HOME
source cfs-env-vars
# Configuration
# Run ./initialize-cfs-bundle once or perform the commands manually.
./install-toolchain
./setup-osal
# Test
./bplib-test-driver  # Runs bplib-unit-test-functional
```

To remove the test artifacts run `./bplib-clean`.

`bplib-clean` prompts to confirm the `rm` command.

