#!/usr/bin/env bash
# Build the firmware and publish/refresh the "latest" GitHub release with firmware.bin.
# The device's OTA flow points at:
#   https://github.com/<owner>/<repo>/releases/latest/download/firmware.bin
#
# Usage:
#   tools/publish_release.sh                       # build + publish
#   tools/publish_release.sh --skip-build          # publish current .pio build
#   tools/publish_release.sh --tag v1.2.3 --notes "..."
set -euo pipefail

repo_root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
firmware_path="$repo_root/.pio/build/waveshare_esp32s3_usb_msc/firmware.bin"
skip_build=0
tag=""
notes=""

while [[ $# -gt 0 ]]; do
  case "$1" in
    --skip-build) skip_build=1; shift ;;
    --tag) tag="${2:-}"; shift 2 ;;
    --notes) notes="${2:-}"; shift 2 ;;
    -h|--help) sed -n '2,9p' "$0" | sed 's/^# \{0,1\}//'; exit 0 ;;
    *) echo "unknown arg: $1" >&2; exit 2 ;;
  esac
done

# Tag first: the builds embed it as RSVP_BUILD_TAG so a device can compare
# its running build against the latest release (Echoform's boot-time OTA).
if [[ -z "$tag" ]]; then
  tag="ota-$(date -u +%Y%m%d%H%M%S)"
fi

echoform_path="$repo_root/.pio/build/echoform/echoform.bin"
if [[ "$skip_build" != "1" ]]; then
  echo "==> building firmware (reader + echoform, tag $tag)"
  RSVP_BUILD_TAG="$tag" ~/.platformio/penv/bin/pio run -d "$repo_root" -e waveshare_esp32s3_usb_msc >/dev/null
  RSVP_BUILD_TAG="$tag" ~/.platformio/penv/bin/pio run -d "$repo_root" -e echoform >/dev/null
fi
# The echoform asset keeps its own name on the shared release channel.
cp "$repo_root/.pio/build/echoform/firmware.bin" "$echoform_path"

[[ -f "$firmware_path" ]] || { echo "firmware.bin not found at $firmware_path" >&2; exit 1; }
[[ -f "$echoform_path" ]] || { echo "echoform.bin not found at $echoform_path" >&2; exit 1; }

[[ -n "$notes" ]] || notes="OTA build $(date -u +'%Y-%m-%d %H:%M UTC')"

repo=$(git -C "$repo_root" config --get remote.origin.url | sed -E 's#.*github.com[:/]([^/]+/[^/.]+)(\.git)?#\1#')
[[ -n "$repo" ]] || { echo "could not derive repo from origin remote" >&2; exit 1; }

echo "==> publishing $tag to $repo (reader $(du -h "$firmware_path" | cut -f1), echoform $(du -h "$echoform_path" | cut -f1))"
gh release create "$tag" "$firmware_path" "$echoform_path" --repo "$repo" --title "$tag" --notes "$notes"

echo "==> released. Device will fetch from https://github.com/$repo/releases/latest/download/firmware.bin"
