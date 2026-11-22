# mini-git

A mini version control system written in C++, inspired by Git. Built for learning how Git works internally.

## Features

- `init`: creates a repository (`.minigit/`)
- `add <file|folder>`: stores files as blob objects
- `commit "msg"`: builds trees and creates a commit
- `log`: shows commit history
- `cat-file <hash>`: prints a stored object

## Build

```bash
g++ -std=c++11 -Wall -o minigit src/*.cpp -lz
```

## Usage

```bash
./minigit init
./minigit add file.txt
./minigit commit "first commit"
./minigit log
```

## How it works

- Objects (Blob, Tree, Commit) are hashed with SHA-1, same format as real Git.
- Objects are compressed with zlib and stored in `.minigit/objects/ab/cdef...`.
- `HEAD` points to `refs/heads/master`, which stores the latest commit hash.
- `log` walks back from HEAD through parent commits.

## Roadmap

- [x] init, add, commit
- [x] log
- [ ] branch, checkout
- [ ] diff
- [ ] merge

Author: jassi-coder43
