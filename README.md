# MiniFileExplorer

MiniFileExplorer is a small terminal-based file manager for Unix-like systems (developed and tested under WSL/Linux). It provides interactive commands to browse, inspect and manipulate files and directories from a simple prompt.

Quick highlights:
- Interactive prompt with persistent current directory
- Basic commands: `ls`, `cd`, `touch`, `mkdir`, `rm`, `rmdir`, `stat`, `search`, `cp`, `mv`, `du`

Quickstart (build & run): Use Makefile to run.
```bash
make
make run
```


Platform & requirements
- C++17 (compiler with <filesystem> support)
- POSIX-like environment (Linux, WSL). Some system calls used assume Unix semantics.

Minimal usage examples (interactive)
- `help` — list available commands
- `ls` — list current directory
- `cd somedir` — change directory
- `touch note.txt` — create empty file
- `rm note.txt` — delete file (prompts for confirmation)
- `exit` — quit the program

Testing & details
- For a full test plan, command reference, and design notes, see `TechnicalDocument.md` (this README intentionally stays brief).
- The project includes a sample manual test suite in `TechnicalDocument.md` that lists both valid and invalid command sequences to exercise error handling.

Contributing / Issues
- If you find bugs or want to improve behavior (e.g., better parsing of quoted paths), open an issue or send a patch.
