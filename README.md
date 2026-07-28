# Task 3 - Memory Management Simulation

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
| Image                     | What it illustrates                                                       |
|---------------------------|---------------------------------------------------------------------------|
| `fifo1.png` & `fifo2.png` | Demonstrates base simulation of FIFO                                      |
| `fifovslru-fifo1.png`     | Case 1: FIFO Working                                                      |
| `fifovslru-lru1.png`      | Case 1: LRU Working                                                       |
| `fifovslru-fifo2.png`     | Case 2: FIFO Working                                                      |
| `fifovslru-lru2.png`      | Case 2: LRU Working                                                       |
| `fifovslru3.png`          | Case 3: Belady's Anomaly                                                  |
| `simulation1.png` & `simulation2.png`      | No Pg. Replacement Algorithm                             |                                     


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
|    Image                  | What it visualizes                                                         |
|---------------------------|----------------------------------------------------------------------------|
| `authorization.png`       | Authorization (password) validation                                        |
| `clientservercommunication.png` | Simple client-server communication                                   |
| `handlingservershutdown.png` | Server Shutdown Handling                                                |
| `malware.png`             | Handling invalid payload length                                            |
| `multiclientcomms.png`    | Concurrent Client Messaging                                                |
| `persistentcomms.png`     | Persistent Communication                                                   | I

## Note

- Password is hardcoded server-side (`"password"`) for demonstration purposes only.
