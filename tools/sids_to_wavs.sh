#!/usr/bin/env bash
# Batch-convert a folder of .sid files into device-friendly WAVs and drop them
# into /Volumes/RSVPNANO/ringtones/.
#
# Usage:
#   tools/sids_to_wavs.sh <source-dir> [destination-dir]
#
# Defaults destination-dir to /Volumes/RSVPNANO/ringtones.
#
# Requires:
#   brew install sidplayfp ffmpeg
#
# Each .sid is rendered to mono 8-bit/8 kHz WAV (~8 KB/sec) — matches the
# device's tiny tinyfont aesthetic and keeps SD usage low even for large
# libraries.

set -euo pipefail

src="${1:-}"
dest="${2:-/Volumes/RSVPNANO/ringtones}"
duration_seconds="${SID_DURATION_SECONDS:-15}"

if [[ -z "$src" ]]; then
  echo "usage: $0 <source-sid-dir> [dest-dir]" >&2
  exit 2
fi
[[ -d "$src" ]] || { echo "source $src is not a directory" >&2; exit 2; }
mkdir -p "$dest"

if ! command -v sidplayfp >/dev/null 2>&1; then
  echo "sidplayfp not found. Install with: brew install sidplayfp" >&2
  exit 1
fi
if ! command -v ffmpeg >/dev/null 2>&1; then
  echo "ffmpeg not found. Install with: brew install ffmpeg" >&2
  exit 1
fi

shopt -s nullglob nocaseglob
mapfile -t sources < <(find "$src" -type f -iname "*.sid" | sort)
shopt -u nocaseglob
if [[ ${#sources[@]} -eq 0 ]]; then
  echo "no .sid files under $src" >&2
  exit 0
fi

tmpdir="$(mktemp -d)"
trap 'rm -rf "$tmpdir"' EXIT

count=0
skipped=0
for src_file in "${sources[@]}"; do
  base="$(basename "$src_file" .sid)"
  base="$(basename "$base" .SID)"
  out="$dest/${base}.wav"
  if [[ -f "$out" ]]; then
    ((skipped++)) || true
    continue
  fi
  intermediate="$tmpdir/${base}.wav"
  echo "==> $base"
  if ! sidplayfp -t"${duration_seconds}" -w"$intermediate" "$src_file" >/dev/null 2>&1; then
    echo "    sidplayfp failed; skipping" >&2
    continue
  fi
  ffmpeg -y -hide_banner -loglevel error \
    -i "$intermediate" -ar 8000 -ac 1 -sample_fmt u8 "$out"
  rm -f "$intermediate"
  ((count++)) || true
done

echo "Converted $count file(s); skipped $skipped already-present."
