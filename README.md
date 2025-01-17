# passutil (WIP)

A simple **work in-progress** command-line utility to store and memorize passwords using libsodium and OpenSSL. Stores passwords in an AES256-encrypted form in a single blob. Metadata is stored separately in an unencrypted form.

**Security IS NOT guaranteed**

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

| **File**                             | **Purpose**                                                                                                                                               |
|--------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------|
| constants/*.h                        | Constants used throughout the code                                                                                                                        |
| passutil.c, passutil.h               | Entry point, argument parsing                                                                                                                             |
| util/string.c, util/string.h         | String utility functions                                                                                                                                  |
| util/io.c, util/io.h                 | IO utility functions                                                                                                                                      |
| util/crypt.c, util/crypt.h           | AES encryption/decryption utilizing OpenSSL                                                                                                               |
| util/shuffle.c, util/shuffle.h       | Password "shuffling" utilities: passwords that are in RAM are stored "shuffled" when possible so as not to expose plain passwords                         |
| util/generation.c, util/generation.h | Generator functions utilizing libsodium                                                                                                                   |
| storage.c, storage.h                 | Store and Password structures, serializers and deserializers, parsers, generators, functions for working with storage and getting passwords in plain text |
| memorizer.c, memorizer.h             | Functions to help memorize the passwords                                                                                                                  |
| transfer.c, transfer.h               | Transfer functions                                                                                                                                        |
| facilities.c, facilities.h           | Generalizations for functions available both as command-line arguments and in interactive mode                                                            |
| pseudoshell.c, pseudoshell.h         | Interactive mode pseudoshell implementation                                                                                                               |

### Password "Shuffling"

Shuffling is the following procedure:

Each symbol of the password is replaced with a different symbol from a symbol pool string. Same symbols are replaced with same symbols. Two different symbols are replaced with two different symbols.

Unshuffling is the reverse of that procedure.

Consider this example: password = `ABE`, shuffle key = `DCFABE`, shuffle key format = `ABCDEF`. This means that the symbols are replaced as following: `A` -> `D`, `B` -> `C`, `C` -> `F`, `D` -> `A`, `E` -> `B` and `F` -> `E`. Shuffled, the password is `DCB`. Unshuffled, it is `ABE` again.

## TODO

Feature-wise:

- [ ] Store key verification (oh boy)
- [X] ~~Add memorization helper to facilities and pseudoshell~~
- [X] ~~Add more memorization options and allow changing them~~
- [ ] Implement transfer functions
- [ ] Add transfer functions to facilities and pseudoshell
- [ ] Add facility-related arguments and make them do somehting (https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html)
- [ ] Windows??? Maybe??? I don't know??? (https://learn.microsoft.com/en-us/windows/console/getconsolemode, https://learn.microsoft.com/en-us/windows/console/console-functions)

Security-wise:

- [X] ~~Use OpenSSL's EVP instead of direct AES functions (https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption)~~
- [X] ~~Use random IVs instead of all zeroes~~
- [ ] Clarify cipher mode and make key hashing function selection an option (make same as key verification algo?)
- [ ] Use some of the store key to encrypt password metadata?
- [ ] Add salt to key before hashing and store it with store metadata
- [ ] Make use of libsodium's security features (secure mallocs, RAM management and stuff)
- [ ] Make more use of password shuffling
- [ ] More cipher types
- [ ] More key verification algorithm types
- [ ] Deny any more arguments after `--interactive` so that user can't be tricked into opening stores, add compile-time option to override this behavior

QoL-wise:

- [ ] Command history so that up/down arrows actually work
- [ ] Command short codes (e.g. so that you can use `q` instead of `quit`)
- [ ] Capture all VT100 escape sequences (https://web.archive.org/web/20121225024852/http://www.climagic.org/mirrors/VT100_Escape_Codes.html)
- [X] ~~Write some actual error messages for facilities~~
- [ ] Capture ^C, kill/term/etc signals, offer to save on any unsaved changes
- [X] ~~`exit` in any mode but store manipulation should probably result in going back to store manipulation mode~~
- [X] ~~In memorization mode, incorrect key presses should result in a "Terminate memorization? (Y/n)" prompt instead of terminating straightaway~~
- [ ] Allow loading stores with explicit separate metadata and master file paths (so that, for example, `metadata.txt` and `masterf` are valid metadata and master file names respectively)
- [ ] Inject debug-related code so that tracking down bugs is less of a nightmare (use preprocessor, only compile debug functions if required)
- [ ] Don't forget to add an actual help message

QoC-wise:

- [ ] Reconsider the usage of `unsigned char**`, separate bytes from chars more clearly
- [ ] Pseudoshell command abstraction
- [ ] Put files serving separate purposes in separate folders (how name?)
- [ ] Better number parsing in pseudoshell (implement in util, use in pseudoshell)
- [ ] Some table-printing functions to reduce clutter in display, peek facilities? (something along the lines of `display.c`, `display.h` probably required)
- [ ] Separate storage constructs better so facilities don't use `store->...`, `password->...` directly
- [ ] Find short variable and function names (e.g. `cnt` which should be `count`) and make them more human-readable
- [ ] Prefix all functions clearly
- [ ] Separate store-related stuff and password-related stuff into `storage/store.c` and `storage/password.c`?

Bug-wise:

- [ ] Track down and fix memory leaks
- [X] ~~Remove VT100 codes from command strings to alleviate file names like "test-something\033[A.psmf"~~
