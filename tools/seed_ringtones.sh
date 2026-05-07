#!/usr/bin/env bash
# Seeds a fresh /ringtones/ folder on a mounted RSVPNANO SD with a starter set
# of RTTTL melodies. Each file holds a single RTTTL string; the filename (minus
# the .rtttl extension) is what shows up in Settings → Tone.
#
# Usage:
#   tools/seed_ringtones.sh                # auto-detect /Volumes/RSVPNANO
#   tools/seed_ringtones.sh /Volumes/X     # explicit destination

set -euo pipefail

dest="${1:-/Volumes/RSVPNANO}"
[[ -d "$dest" ]] || { echo "destination $dest is not mounted" >&2; exit 1; }
mkdir -p "$dest/ringtones"

write() {
  local name="$1"
  local rtttl="$2"
  printf '%s' "$rtttl" > "$dest/ringtones/${name}.rtttl"
  echo "  + ${name}.rtttl"
}

write "Nokia"       'Nokia:d=4,o=5,b=180:8e6,8d6,f#,g#,8c#6,8b,d,e,8b,8a,c#,e,2a'
write "Meshtastic"  'Meshtastic:d=32,o=5,b=565:f6,p,f6,4p,p,f6,p,f6,2p,p,b6,p,b6,p,b6,p,b6,p,b,p,b,p,b,p,b,p,b,p,b,p,b,p,b,1p.,2p.,p'
write "Zelda"       'ZeldaGetIt:d=16,o=5,b=120:g,c6,d6,2g6'
write "MarioCoin"   'MarioCoin:d=8,o=5,b=125:b5,2e6'
write "Beep"        'Beep:d=8,o=6,b=180:c,p,c'

echo "Seeded $(ls "$dest/ringtones" | wc -l | tr -d ' ') ringtones in $dest/ringtones"
