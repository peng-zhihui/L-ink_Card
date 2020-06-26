Contributing
============

We appreciate your contributions! Because this is an open source project, we want to keep it as easy
as possible to contribute changes. However, we need contributors to follow a few guidelines.


## Requirements

There are a few architectural requirements that must be followed by contributed changes.

- Target-specific code is not allowed in common source files. Specifically, this means that you
    may not introduce `#ifdefs` for the target or HIC into common code.
- If you need to implement a target-specific feature that would require changes to common code, you
    must first add a hook mechanism to the common code that allows your target-specific feature to
    be implemented outside the common code (in a family or board source file).
- DAPLink makes extensive use of weak function and variable definitions as hooks to allow
    target-specific builds to change behaviour. This is the preferred hook mechanism.
- Another option is to use a macro defined in the board yaml to override a default. In such a
    design, there must be a reasonable default if the macro is not set.


## Coding style

Contributed source code must match the existing style of the DAPLink code base.

The key requirements are:
- 4 space indents, no tabs are allowed.
- No trailing whitespace.
- All source files must end with a newline (an empty line at the end).
- Lines should generally not be longer than 120 characters, though this is not a hard rule.
- Braces generally follow the K&R style.
- Single-statement blocks for **if**, **else**, **do**, **while**, and **for** statements must be
    enclosed in braces.
- Spaces are required between keywords and parentheses, and around operators.
- All functions require prototypes.
- Macros must be all uppercase.
- Functions should be all lowercase with underscores between words (sometimes called snake case).
- Types must be all lowercase, underscores between words, and have a `_t` suffix.
- Enumerations must start with `k` and use camel case, e.g. `kHardwareReset`.
- Code must be well commented and use Doxygen-style documentation comments for all declarations.

For other style questions, please look to the existing code for answers.


## License

By creating a pull request on GitHub, you are granting license your contributions under the same
Apache 2.0 license as the original code. However, you still own the copyright to your contributions.
You may add your copyright below the Arm copyright in the license header at the top of the source
file. If a file is newly added by you, it must contain the standard license header, but it does not
require an Arm copyright. Please note that we do not list changes in each source file by copyright
owner, as this becomes a burden to maintain.


## Process

Please create a pull request in GitHub with your contribution. Before creating the pull request,
please ensure that your code meets the requirements listed above and all tests pass. We also run the
tests on a wide range of boards for every pull request using our CI setup. Changes must pass on all
tested boards before the the pull request can be merged.

The [developers' guide](docs/DEVELOPERS-GUIDE.md) describes how to create your development
environment. The [automated tests guide](docs/AUTOMATED_TESTS.md) provides information about the
available types of tests and describes how to run the tests.


## More

For more information about contributing, see the Mbed OS [contributor
documentation](http://os.mbed.com/contributing). Although this documentation is written primarily
with Mbed OS in mind, much of it applies directly to DAPLink, as well.
