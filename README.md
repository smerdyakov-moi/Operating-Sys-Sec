# Libraries & Headers Used by Task

## Task 1

| Library / Header | Purpose in this task |
|---|---|
| `<pthread.h>` | `pthread_create()`, `pthread_join()`, `pthread_mutex_t`, `pthread_mutex_lock()`/`unlock()` — threading and mutex synchronization for the flight threads and round-robin `turn_lock` |
| `<unistd.h>` | `fork()`, `sleep()` — process creation and the deliberate delays used to make race conditions and deadlocks reproducible |
| `<sys/wait.h>` | `wait()` — parent process blocking on the forked child |
| `<stdlib.h>` | General process/utility support (exit codes, standard allocation) |

**Link flag:** `-lpthread` (or `-pthread`) required at compile time for POSIX threads.

## Task 2

| Library / Header | Purpose in this task |
|---|---|
| `<stdint.h>` | Fixed-width integer types for virtual/physical address arithmetic (bitwise decomposition into page number/offset) |
| `<stdlib.h>` | Array/memory management for `physical_frames[]`, `last_time_used[]`, etc. |

No third-party or POSIX-specific libraries are required — this task is implemented with standard C only, since it is a single-threaded simulation of address translation and page replacement (FIFO/LRU).

## Task 3

| Library / Header | Purpose in this task |
|---|---|
| `<string.h>` | `strcmp()` for login credential comparison; string handling for usernames, filenames, and permissions |
| `<stdlib.h>` | Management of the in-memory `users[]` and `VirtualFile files[MAX_FILES]` arrays |
| `<time.h>` | Timestamps recorded in each `audit.log` entry |

This task is single-process/in-memory (no threading library needed); its only disk I/O is the append-only `audit.log` file.

## Task 4

| Library / Header | Purpose in this task |
|---|---|
| `<sys/socket.h>` | Core socket API — `socket()`, `bind()`, `listen()`, `accept()`, `connect()`, `send()`/`recv()` for the TCP client-server protocol |
| `<netinet/in.h>` | `sockaddr_in`, port/address structures for binding to TCP port 4000 |
| `<arpa/inet.h>` | Address conversion utilities (e.g., `inet_pton()`/`inet_ntoa()`) for client-server addressing |
| `<pthread.h>` | `pthread_create()` for spawning a detached thread per connected client; `pthread_detach()` for automatic resource reclamation |
| `<unistd.h>` | `read()`/`write()`/`close()` on socket file descriptors |
| `<string.h>` | Packet payload handling, `custom_crypt()` XOR routine over message buffers |

**Link flag:** `-lpthread` required for the multi-threaded server (`gcc server.c -o s1 -lpthread`).

---


# Task 1 - Process Management and Threading


A set of C programs demonstrating process creation, thread management, and
synchronization using a shared scenario: three flights (5, 11, 19) landing on
a shared runway/taxiway.

## Files

| File | Demonstrates |
|---|---|
| `landingwithoutsync.c` | Race condition (no lock on shared state) |
| `landingwithsync.c` | Fix using a mutex (critical section) |
| `roundrobin.c` | Round-robin CPU scheduling simulation |
| `deadlock.c` | Deadlock via circular wait (two locks, reversed order) |
| `deadlockprevention.c` | Deadlock prevention via fixed lock ordering |
| `finalprogram.c` | Integrated program: round-robin descent + deadlock-free landing |

## Requirements

- GCC
- POSIX threads (`pthread`)
- Linux/macOS (uses `fork()`)

## Build

```bash
gcc  landingwithoutsync.c   -o landingwithoutsync
gcc  landingwithsync.c      -o landingwithsync
gcc  roundrobin.c           -o roundrobin
gcc  deadlock.c             -o deadlock
gcc  deadlockprevention.c   -o deadlockprevention
gcc  finalprogram.c         -o finalprogram
```

## Run

```bash
./landingwithoutsync
./landingwithsync
./roundrobin
./deadlockprevention
./finalprogram
```

> **Note:** `./deadlock` will hang intentionally (it demonstrates a real
> deadlock via circular wait). Press `Ctrl+C` to stop it.

## Project Structure

Each program follows the same base pattern:

1. `main()` calls `fork()` to create a child process.
2. The child process spawns 3 threads (`pthread_create`), one per flight.
3. The child waits for all threads to finish (`pthread_join`).
4. The parent waits for the child (`wait`).

Shared resources (runway/taxiway locks, scheduler turn) are protected with
`pthread_mutex_t` where synchronization is required.

## Screenshots
| Image| What it demonstrates |
|---|---|
| `deadlock.png` | Deadlock Terminal Hang |
| `deadlockprevention.png` | Fix to deadlock |
| `landingwithoutsync.png` | Airplane Collision |
| `landingwithsync.png` | Synchronized landing |
| `roundrobin.png` | Round Robin Simulation |

# Task 2 - Memory Management Simulation

A small C project that simulates paging in a virtual memory system: address
translation, page hits/faults, and two page replacement algorithms
(FIFO and LRU).

## Files

| File                     | What it does                                                              |
|--------------------------|---------------------------------------------------------------------------|
| `pagingsys.c`            | Baseline: translation + hit/fault detection, **no** replacement policy    |
| `pagingsyswithfifo.c`    | Same as above, plus **FIFO** page replacement                             |
| `pagingsyswithlru.c`     | Same as above, plus **LRU** page replacement                              |
| `final1.c`               | Test Case 1: LRU outperforms FIFO (equal frames)                          |
| `final2.c`               | Test Case 2: FIFO and LRU perform identically                             |
| `final3.c`               | Test Case 3: Belady's Anomaly: Increase in frame hampers FIFO performance |

## Configuration

Page size, process size, and physical memory size are set at the top of each
file and can be changed freely:

```c
#define PROCESS_SIZE 8192   // virtual address space of the process
#define MEM_SIZE     4096   // physical RAM available
#define OFFSET_BIT   10     // page size = 2^OFFSET_BIT bytes
```

Everything else (number of pages, number of frames) is calculated from these
three values, so changing `OFFSET_BIT` changes the page size everywhere
automatically.

## Build & Run

```bash
gcc pagingsys.c -o pagingsys
./pagingsys

gcc pagingsyswithfifo.c -o pagingsyswithfifo
./pagingsyswithfifo

gcc pagingsyswithlru.c -o pagingsyswithlru
./pagingsyswithlru

gcc final1.c -o final1
./final1

gcc final2.c -o final2
./final2

gcc final3.c -o final3
./final3
```

## What gets printed

For every requested virtual address, the program prints:
- the page number and offset it translates to
- whether it was a **hit** or a **fault**
- which frame it landed in (or which frame was evicted, once a replacement
  policy is active)
- the resulting physical address
- a snapshot of every frame's contents so far

At the end, it prints the overall **hit ratio** and **fault ratio**.

## FIFO vs. LRU

- **FIFO** evicts whichever page has been resident the longest, tracked with
  a single pointer that only moves forward. It doesn't care whether that page
  was just used a moment ago.
- **LRU** evicts whichever page hasn't been *used* the longest, tracked with a
  per-frame "last used" timestamp that's refreshed on every hit.

On the workload in `final.c`, LRU gets roughly double the hit ratio of FIFO,
because it correctly protects a page that was reused shortly before an
eviction was forced — something FIFO has no way to notice.

## Screenshots
| Image                     | What it demonstrates                                                      |
|---------------------------|---------------------------------------------------------------------------|
| `fifo1.png` & `fifo2.png` | Demonstrates base simulation of FIFO                                      |
| `fifovslru-fifo1.png`     | Case 1: FIFO Working                                                      |
| `fifovslru-lru1.png`      | Case 1: LRU Working                                                       |
| `fifovslru-fifo2.png`     | Case 2: FIFO Working                                                      |
| `fifovslru-lru2.png`      | Case 2: LRU Working                                                       |
| `fifovslru3.png`          | Case 3: Belady's Anomaly                                                  |
| `simulation1.png` & `simulation2.png`      | No Pg. Replacement Algorithm                             |                                     


# Task 3 - Virtual OS — Simple File Management System (C)

A command-line simulation of a small multi-user operating system, built in C. It models user authentication, an owner/group/others file permission scheme, basic file encryption, and audit logging, all backed by in-memory arrays (no real disk I/O).

## Features

- **User authentication** — login/logout with username + password
- **User management** — root-only account creation and deletion (`adduser`, `removeuser`)
- **Virtual file system** — create, read, write, and delete files held in memory
- **Permissions** — owner / group / others read, write, execute flags per file
- **Encryption** — XOR-based encrypt/decrypt on file contents
- **Audit logging** — every login, file operation, and permission decision is appended to `audit.log`

## Files

| File | Purpose |
|---|---|
| `final.c` | Entry point; command loop and user input handling |
| `auth.c` / `auth.h` | Login, logout, session state |
| `users.c` / `users.h` | Account creation/deletion (root-only) |
| `filesystem.c` / `filesystem.h` | File create/read/write/delete, permissions, encryption |
| `audit.c` / `audit.h` | Append-only action logging to `audit.log` |
| `globals.c` / `globals.h` | Shared structs, constants, and global state |

## Build

```bash
gcc -o virtualos final.c auth.c users.c filesystem.c audit.c globals.c
```

## Run

```bash
./virtualos
```

On first launch, a default `root` account is created automatically (see `initialize_system()` in `auth.c`).

## Commands

| Command | Description |
|---|---|
| `login` | Log in with a username and password |
| `logout` | End the current session |
| `adduser` | Create a new account (root only) |
| `removeuser` | Delete an account and its files (root only) |
| `create` | Create a new file with default permissions |
| `read` | Read a file's contents (permission-checked) |
| `write` | Overwrite a file's contents (permission-checked) |
| `delete` | Delete a file (owner or root only) |
| `encrypt` | XOR-encrypt a file's contents |
| `dump` | Show raw in-memory file bytes (root only) |
| `exit` | Shut down the program |

## Notes

- All data is in-memory only — nothing persists between runs except `audit.log`.
- This project was built for coursework and includes several intentional/discovered security weaknesses (unbounded input reads, plaintext password storage, a weak XOR cipher, and others) — see the accompanying security analysis report for details.

# Task 4 — Network Programming and IPC (TCP Client-Server Chat)

A multithreaded TCP client-server application demonstrating socket-based
inter-process communication, a custom framed protocol, authentication,
and basic security validation.

## Files

| File | Description |
|---|---|
| `server.c` | Multithreaded TCP server — one detached `pthread` per connected client |
| `client.c` | Interactive TCP client — authenticates, then sends messages in a loop |
| `malware.c` | Standalone test client used to trigger the server's payload-length validation (sends a forged header only, bypassing normal message input) |

## Build

```bash
gcc server.c -o s1 -lpthread
gcc client.c -o c1
gcc client.c -o c2
gcc malware.c -o mal_client   # optional, for the validation test
```


## Run

1. Start the server first (it listens on port `4000`):
   ```bash
   ./s1
   ```
2. In a separate terminal, start one or more clients:
   ```bash
   ./c1
   ```

   ``` bash
   ./c2
   ```
3. When prompted, enter the password: `password`
4. After authentication, type messages to send. Type `exit` to close the session cleanly.

Multiple clients can connect at the same time (`./c1`, `./c2`, ...) — the
server handles each on its own thread.

## Protocol Summary

- Transport: TCP (`SOCK_STREAM`) on port `4000`.
- Every message is preceded by a fixed 44-byte `PacketHeader`:
  ```c
  typedef struct {
      char command[8];        // "AUTH", "MSG", or "EXIT"
      int  payload_length;    // validated: 0 <= n < 512
      char session_token[32];
  } PacketHeader;
  ```
- Flow: `AUTH` (must succeed before anything else) → repeated `MSG` /
  `SERVER_RECEIVED_OK` request-acknowledgement pairs → `EXIT` to close.
- Payloads are XOR-encrypted (key `0x5A`) via `custom_crypt()` in both directions.

## Security Measures

- **Authentication gate** — no command is processed until a valid `AUTH` packet is received.
- **Payload-length validation** — the server rejects any header where `payload_length < 0` or `>= 512`, before reading the payload. This check runs even before authentication.
- **XOR obfuscation** of passwords and messages on the wire.
- **`MSG_NOSIGNAL`** on every `send()` to avoid `SIGPIPE` crashes on a dropped peer.
- **Read-return checks** (`<= 0`) on every `read()` to detect disconnects instead of crashing.

## Testing the Security Check

The normal client can never send an out-of-range `payload_length`, since it's
derived from `strlen()` on a 512-byte buffer. To test the server's validation,
run `mal_client`, which sends only a header with `payload_length = 9999`:

```bash
./mal_client    # terminal 2
```

Expected server output:
```
||SECURITY BLOCKED|| Malicious payload boundary detected: 9999 bytes!
```

## Screenshots
|    Image                  | What it demonstrates                                                       |
|---------------------------|----------------------------------------------------------------------------|
| `authorization.png`       | Authorization (password) validation                                        |
| `clientservercommunication.png` | Simple client-server communication                                   |
| `handlingservershutdown.png` | Server Shutdown Handling                                                |
| `malware.png`             | Handling invalid payload length                                            |
| `multiclientcomms.png`    | Concurrent Client Messaging                                                |
| `persistentcomms.png`     | Persistent Communication                                                   | I

## Note

- Password is hardcoded server-side (`"password"`) for demonstration purposes only.