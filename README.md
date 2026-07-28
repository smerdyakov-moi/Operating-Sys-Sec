
# Task 4 — Network Programming and IPC (TCP Client-Server Chat)

A multithreaded TCP client-server application demonstrating socket-based
inter-process communication, a custom framed protocol, authentication,
and basic security validation.

## Files

| File | Description |
|---|---|
| `server.c` | Multithreaded TCP server — one detached `pthread` per connected client |
| `client.c` | Interactive TCP client — authenticates, then sends messages in a loop |
| `malicious_client.c` | Standalone test client used to trigger the server's payload-length validation (sends a forged header only, bypassing normal message input) |

## Build

```bash
gcc server.c -o s1 -lpthread
gcc client.c -o c1
gcc malicious_client.c -o mal_client   # optional, for the validation test
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
./s1            # terminal 1
./mal_client    # terminal 2
```

Expected server output:
```
||SECURITY BLOCKED|| Malicious payload boundary detected: 9999 bytes!
```

## Notes

- Password is hardcoded server-side (`"password"`) for demonstration purposes only.
- XOR encryption and the fixed credential are simplified stand-ins for
  TLS and a real credential store, sufficient to demonstrate the required
  security-measure concept at the protocol level for this assignment.