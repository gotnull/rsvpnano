#!/usr/bin/env python3
"""Clone configured public-domain book repos and copy unique .epub files
into the project's assets/books/ folder.

Dedup strategy: skip if (a) a file with the same SHA-256 is already present, or
(b) a file whose normalized title is already present. Normalization lowercases
the basename, drops punctuation, and trims common suffixes like
"- book 1" / "(unabridged)".
"""
from __future__ import annotations

import hashlib
import re
import shutil
import subprocess
import sys
from pathlib import Path

REPOS = [
    "https://github.com/stepbasin/books.git",
    "https://github.com/popkornov/literature-epub.git",
]

CACHE_DIR = Path.home() / ".cache" / "rsvpnano-books"


def normalize(name: str) -> str:
    stem = name.rsplit(".", 1)[0].lower()
    stem = re.sub(r"\(.*?\)", " ", stem)
    stem = re.sub(r"\b(book|vol|volume|part)\s*\d+\b", " ", stem)
    stem = re.sub(r"[^a-z0-9]+", "", stem)
    return stem


def hash_file(path: Path) -> str:
    h = hashlib.sha256()
    with path.open("rb") as f:
        for chunk in iter(lambda: f.read(1 << 20), b""):
            h.update(chunk)
    return h.hexdigest()


def clone_or_update(url: str) -> Path:
    name = url.rstrip("/").rsplit("/", 1)[-1].removesuffix(".git")
    target = CACHE_DIR / name
    if (target / ".git").is_dir():
        print(f"  [cached] {name} (rm -rf {target} to refresh)")
        return target
    print(f"  [clone]  {name}")
    target.parent.mkdir(parents=True, exist_ok=True)
    subprocess.run(
        ["git", "clone", "--quiet", "--depth", "1", url, str(target)],
        check=True,
        timeout=300,
    )
    return target


def main(dest_dir: Path) -> int:
    dest_dir.mkdir(parents=True, exist_ok=True)
    seen_norm: set[str] = set()
    seen_hash: set[str] = set()

    print(f"==> indexing existing books in {dest_dir}")
    for existing in dest_dir.glob("*.epub"):
        seen_norm.add(normalize(existing.name))
        seen_hash.add(hash_file(existing))

    added = 0
    skipped_norm = 0
    skipped_hash = 0
    for url in REPOS:
        repo_dir = clone_or_update(url)
        for source in repo_dir.rglob("*.epub"):
            if not source.is_file():
                continue
            norm = normalize(source.name)
            if norm in seen_norm:
                skipped_norm += 1
                continue
            digest = hash_file(source)
            if digest in seen_hash:
                skipped_hash += 1
                continue
            target = dest_dir / source.name
            if target.exists():
                target = dest_dir / f"{source.stem}-{digest[:8]}{source.suffix}"
            shutil.copy2(source, target)
            seen_norm.add(norm)
            seen_hash.add(digest)
            added += 1

    print(
        f"==> added {added} epub(s); skipped {skipped_norm} by name, "
        f"{skipped_hash} by content"
    )
    return 0


if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("usage: pull_extra_books.py <dest_dir>", file=sys.stderr)
        raise SystemExit(2)
    raise SystemExit(main(Path(sys.argv[1]).expanduser().resolve()))
