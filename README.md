# File Sharing over TCP Sockets (C)

A minimal client–server file-transfer tool written from scratch in C using
POSIX sockets. The **client** sends any file — text, PDF, image, anything — to
the **server**, which saves it to disk byte-for-byte intact.

Built as a hands-on way to learn how network programming actually works, from
the socket API up to the little "protocol" that keeps the bytes in order.

## Features

- Transfers **any file type and size** (binary-safe — verified with PDFs).
- A simple **length-prefixed protocol** so the receiver learns the original filename.
- **Robust I/O** — `send_all` / `recv_all` loops that never drop bytes on partial reads/writes.
- Works over **localhost or a real network** by pointing the client at the server's IP.

## How it works

TCP is a *stream* — data arrives in arbitrary chunks, so the two sides agree on
a tiny protocol that frames the pieces. For each transfer the client sends:

```
[ 4 bytes ]  length of the filename   (a 32-bit integer, network byte order)
[ N bytes ]  the filename
[ ...     ]  the file's contents, until the connection closes
```

The receiver reads the first 4 bytes to learn the filename length, reads exactly
that many bytes for the name, then reads the rest of the stream (until the sender
closes the connection) as the file's contents.

The socket flow is the classic pair:

- **Server (receiver):** `socket → bind → listen → accept → recv → write to disk`
- **Client (sender):** `socket → connect → send`

Multi-byte numbers (the port and the filename length) are converted to *network
byte order* with `htons` / `htonl` so the transfer works regardless of each
machine's native byte order.

## Requirements

- macOS or Linux
- A C compiler (`gcc` or `clang`)

## Build

```bash
gcc server.c -o server
gcc client.c -o client
```

## Usage

Open two terminals in this folder.

**Terminal 1 — start the receiver:**

```bash
./server
```

It binds to port `5001` and waits for a connection.

**Terminal 2 — send a file:**

```bash
./client path/to/yourfile.pdf
```

The server saves the incoming file in its folder with a `received_` prefix
(e.g. `received_yourfile.pdf`) so it never overwrites the original.

### Choosing the server address

The server's IP is set in `client.c` (the `inet_pton(...)` line):

- **Same machine (testing):** use `127.0.0.1` (localhost).
- **Across a network:** use the server machine's IP on your Wi-Fi/LAN
  (find it on the server with `ipconfig getifaddr en0` on macOS, or
  `hostname -I` on Linux). Both machines must be on the same network, and the
  server's firewall must allow incoming connections on port `5001`.

> Note: port `5000` is avoided because modern macOS reserves it for AirPlay
> Receiver; this project uses `5001`.

## Project structure

```
.
├── server.c   # the receiver: listens, accepts, and saves the incoming file
├── client.c   # the sender: connects and streams a file
└── README.md
```

## Concepts practiced

Sockets and the client/server model · TCP vs UDP · file descriptors and the
kernel's per-process descriptor table · `struct sockaddr_in`, `htons`/`htonl`,
`INADDR_ANY` · pointers, casts, and buffers · why a single `recv`/`send` can be
partial (and how looping fixes it) · designing a length-prefixed wire protocol ·
binary-safe file I/O.

## Possible improvements

- Take the server IP (and port) as command-line arguments instead of hardcoding.
- Send the file size up front to show a progress bar.
- Add cross-platform support for Windows (Winsock) using `#ifdef _WIN32` blocks.
- Serve multiple clients (loop back to `accept`, or use threads).
- Preserve the exact original filename (and fix the `received_` prefix spelling).

---

*A learning project — built step by step to understand network programming in C,
not just to make it work.*
