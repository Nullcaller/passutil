# passutil (WIP)

A simple **WORK IN-PROGRESS** command-line utility to store and memorize passwords using libsodium and OpenSSL. Stores passwords in an AES256-encrypted form in a single blob. Metadata is stored separately in an encrypted form.

By default, no AES256 encryption key validation methods are avilable. `passutil` relies on the user to recognize whether the presented passwords are correct. This way, if the passwords are generated with `passutil`, a potential attacker with access to the password store will pretty much be unable to formulate a hypothesis when attempting to decrypt the passwords. At least, that's the hope.

All of the code is written with the goal of being humanly readable, to make checking it easier.

**SECURITY IS NOT GUARANTEED**

Memory leaks are currently **guaranteed**.

Data loss *is not guranateed*. However, **data loss *is* highly likely**.

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

* Achieve MVP state
* Fix all memory leaks
* Make use of libsodium's security features (secure mallocs, RAM managements and stuff)
* Encrypt metadata with a separate password
* Make more use of password shuffling
* Use OpenSSL's EVP instead of direct AES functions
