# Scratchy — Multi-threaded Concurrent HTTP/1.0 Request Handler over Raw TCP/IPv4 Sockets

*Part of the Scratchy series: building foundational software from first principles, one layer at a time.*

---

## Current State
Building the tcp sockets and establishing the connection using Go (net)

---

## What Is This

An HTTP server built entirely from scratch using Python's low-level socket module and Go — no frameworks, no abstractions. The goal is to understand exactly what happens underneath libraries like Flask or Django.

---

## Philosophy

Start with the smallest thing that works. Get one client talking to one server. Then add just enough to handle real HTTP. Then add concurrency. No step is skipped.

---

## Stack

Python — standard library only. No third-party packages. The `socket` module handles all network I/O, `threading` will handle concurrency, and `json` handles body serialization.

Go — using net package, for certain functionalities and possibly the whole program.

---

## Project Stages

- **Stage 1** — Raw socket communication: server binds, listens, accepts, sends. *(Done)*
- **Stage 2** — HTTP compliance: parse real HTTP requests, respond with correct HTTP format including status line, headers, and body.
- **Stage 3** — Routing: map paths to handlers, return different responses based on the requested URL.
- **Stage 4** — Concurrency: introduce threading so multiple clients can connect simultaneously without blocking each other.

---

## Goal

A multi-threaded HTTP server/request handler with a companion script that spawns multiple simultaneous clients — each connecting, sending a request, and receiving a response — to demonstrate the server handling real concurrent load.

---

## The Scratchy Series

Scratchy is a personal project series dedicated to rebuilding common software tools without relying on the libraries that normally hide their complexity. Each project in the series targets one layer of the stack.
