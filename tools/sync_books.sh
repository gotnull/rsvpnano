#!/usr/bin/env bash
# Convert books in assets/books/ to .rsvp and sync them to the SD card.
#
# Usage:
#   tools/sync_books.sh                # auto-detect SD volume under /Volumes
#   tools/sync_books.sh --dest <path>  # explicit destination (e.g. /Volumes/RSVP)
#   tools/sync_books.sh --force        # re-convert even if .rsvp already exists
#   tools/sync_books.sh --pull-extras  # also clone configured book repos and copy unique .epub files
#
# Source files (.epub, .txt, .html, .md, ...) live in assets/books/.
# Converted .rsvp files are written next to their sources, then copied
# into <dest>/books/ on the SD card. If no SD volume is mounted, output
# is staged in build/sd_root/books/ for manual copy.

set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
src_dir="$repo_root/assets/books"
converter="$repo_root/tools/sd_card_converter/convert_books.py"
fallback_dest="$repo_root/build/sd_root"

dest_override=""
force_flag=""
pull_extras=0
while [[ $# -gt 0 ]]; do
  case "$1" in
    --dest)
      dest_override="${2:-}"
      [[ -n "$dest_override" ]] || { echo "--dest needs a path" >&2; exit 2; }
      shift 2
      ;;
    --dest=*)
      dest_override="${1#--dest=}"
      shift
      ;;
    --force)
      force_flag="--force"
      shift
      ;;
    --pull-extras)
      pull_extras=1
      shift
      ;;
    -h|--help)
      sed -n '2,13p' "$0" | sed 's/^# \{0,1\}//'
      exit 0
      ;;
    *)
      echo "unknown arg: $1" >&2
      exit 2
      ;;
  esac
done

if [[ ! -d "$src_dir" ]]; then
  echo "creating $src_dir"
  mkdir -p "$src_dir"
fi

python_bin="${PYTHON:-python3}"
command -v "$python_bin" >/dev/null || { echo "python3 not found" >&2; exit 1; }

if [[ "$pull_extras" == "1" ]]; then
  echo "==> pulling extra book repos (deduped) into $src_dir"
  "$python_bin" "$repo_root/tools/pull_extra_books.py" "$src_dir"
fi

shopt -s nullglob
sources=("$src_dir"/*.epub "$src_dir"/*.txt "$src_dir"/*.html "$src_dir"/*.htm "$src_dir"/*.xhtml "$src_dir"/*.md "$src_dir"/*.markdown)
shopt -u nullglob
if [[ ${#sources[@]} -eq 0 ]]; then
  echo "no source books in $src_dir — drop .epub/.txt/.html/.md files there and re-run"
  exit 0
fi

echo "==> converting ${#sources[@]} source file(s) in $src_dir"
"$python_bin" "$converter" --root "$src_dir" --no-pause $force_flag

resolve_dest() {
  if [[ -n "$dest_override" ]]; then
    echo "$dest_override"
    return
  fi
  local candidates=()
  shopt -s nullglob
  for vol in /Volumes/*; do
    [[ -d "$vol" && -w "$vol" ]] || continue
    [[ "$vol" == "/Volumes/Macintosh HD"* ]] && continue
    candidates+=("$vol")
  done
  shopt -u nullglob
  if [[ ${#candidates[@]} -eq 1 ]]; then
    echo "${candidates[0]}"
  elif [[ ${#candidates[@]} -gt 1 ]]; then
    echo "multiple removable volumes mounted — pass --dest <path>:" >&2
    printf '  %s\n' "${candidates[@]}" >&2
    exit 3
  fi
}

dest="$(resolve_dest)"
if [[ -z "$dest" ]]; then
  dest="$fallback_dest"
  echo "==> no SD volume mounted; staging to $dest"
  mkdir -p "$dest"
fi

books_dest="$dest/books"
mkdir -p "$books_dest"

shopt -s nullglob
rsvp_files=("$src_dir"/*.rsvp)
shopt -u nullglob
if [[ ${#rsvp_files[@]} -eq 0 ]]; then
  echo "no .rsvp files produced (conversion may have failed)" >&2
  exit 1
fi

echo "==> copying ${#rsvp_files[@]} .rsvp file(s) to $books_dest"
rsync -av --human-readable "${rsvp_files[@]}" "$books_dest/"

echo "done."
