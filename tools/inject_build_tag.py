"""Inject a UTC build-time tag as a -D flag so the firmware can show which
build is actually running. Helps diagnose stale-OTA / anti-rollback situations
where the device isn't on the version we just published."""
import datetime
import os

Import("env")

tag = os.environ.get("RSVP_BUILD_TAG") or datetime.datetime.utcnow().strftime("%y%m%d-%H%M%S")
env.Append(BUILD_FLAGS=[f'-DRSVP_BUILD_TAG=\\"{tag}\\"'])
print(f"-- RSVP build tag: {tag}")
