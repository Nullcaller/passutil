# passutil (WIP)

A simple **work in-progress** command-line utility to store and memorize passwords using libsodium and OpenSSL. Stores passwords in an AES256-encrypted form in a single blob. Metadata is stored separately in an unencrypted form.

**SECURITY IS NOT GUARANTEED**

Memory leaks are practically **guaranteed** and the code is **spaghetti**.

Data loss *is not guranateed*. However, **data loss *is* highly likely**.

If you still have doubts about whether or not secuirty is guaranteed, please take a read at section 4b of this project's license file:

```
4. Limitations and Disclaimers.

<...>

 b. Affirmer offers the Work as-is and makes no representations or
    warranties of any kind concerning the Work, express, implied,
    statutory or otherwise, including without limitation warranties of
    title, merchantability, fitness for a particular purpose, non
    infringement, or the absence of latent or other defects, accuracy, or
    the present or absence of errors, whether or not discoverable, all to
    the greatest extent permissible under applicable law.
```

Please, do believe every word it says and review the code yourself if you have any questions.

By default, no encryption key validation methods are avilable. `passutil` relies on the user to recognize whether the presented passwords are correct. This way, if the passwords are generated with `passutil`, a potential attacker with access to the password store will pretty much be unable to formulate a hypothesis when attempting to decrypt the passwords. At least, that's the hope.

All of the code is written with the goal of being humanly readable, to make checking it easier. This results in variable names akin to `first_argument_except_for_the_command_argument_except_that_weird_edgecase_where...`. This is intentional. And keep in mind: human-readable code is the goal, but there is no guarantee that this goal will ever be achieved.

## Project Structure

| **File**                     | **Purpose**                                                                                                                                                       |
|------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| constants-*.h                | Constants used throughout the code                                                                                                                                |
| passutil.c, passutil.h       | Entry point, argument parsing                                                                                                                                     |
| util.c, util.h               | Utility functions: string functions, direct file bytes read/write, AES encryption/decryption (to be moved)                                                        |
| shuffler.c, shuffler.h       | Password "shuffling" utilities: passwords that are in RAM are stored "shuffled" when possible so as not to expose plain passwords                                 |
| generation.c, generation.h   | Generator functions for shuffle keys and passwords utilizing libsodium                                                                                            |
| storage.c, storage.h         | All things storage: Store and Password structures, serializers and deserializers, parsers, functions for working with storage and getting passwords in plain text |
| memorizer.c, memorizer.h     | Functions to help memorize the passwords                                                                                                                          |
| transfer.c, transfer.h       | Transfer functions                                                                                                                                                |
| facilities.c, facilities.h   | Generalizations for functions available both as command-line arguments and in interactive mode                                                                    |
| pseudoshell.c, pseudoshell.h | Interactive mode pseudoshell implementation                                                                                                                       |

### Password "Shuffling"

Shuffling is the following procedure:

Each symbol of the password is replaced with a different symbol from a symbol pool string. Same symbols are replaced with same symbols. Two different symbols are replaced with two different symbols.

Unshuffling is the reverse of that procedure.

Consider this example: password = `ABE`, shuffle key = `DCFABE`, shuffle key format = `ABCDEF`. This means that the symbols are replaced as following: `A` -> `D`, `B` -> `C`, `C` -> `F`, `D` -> `A`, `E` -> `B` and `F` -> `E`. Shuffled, the password is `DCB`. Unshuffled, it is `ABE` again.

## TODO

* Implement all mentioned features (memorization helper and transfer are not currently implemented)
* Fix memory leaks
* Make use of libsodium's security features (secure mallocs, RAM managements and stuff)
* Encrypt metadata with a separate password?
* Make more use of password shuffling
* Use OpenSSL's EVP instead of direct AES functions
