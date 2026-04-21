# Post-build: write version.txt next to firmware.bin (OTA server file).
# Version string is taken from #define FIRMWARE_VERSION in src/config/config.h.
# To change what ships in the build, edit FIRMWARE_VERSION there; after build,
# edit .pio/build/<env>/version.txt only if you want a different value on the server
# without rebuilding (OTA compares server file to flashed firmware).

import os
import re

Import("env")


def _read_firmware_version(config_h_path):
    try:
        with open(config_h_path, "r", encoding="utf-8") as fh:
            text = fh.read()
    except OSError as exc:
        print("post_version_txt: could not read config.h:", exc)
        return "0.0.0"
    m = re.search(r'^\s*#define\s+FIRMWARE_VERSION\s+"([^"]*)"', text, re.MULTILINE)
    if not m:
        print("post_version_txt: FIRMWARE_VERSION not found in config.h")
        return "0.0.0"
    return m.group(1)


def write_version_txt(source, target, env):
    project_dir = env.subst("$PROJECT_DIR")
    build_dir = env.subst("$BUILD_DIR")
    config_h = os.path.join(project_dir, "src", "config", "config.h")
    out_path = os.path.join(build_dir, "version.txt")

    version = _read_firmware_version(config_h)
    with open(out_path, "w", encoding="utf-8", newline="\n") as fh:
        fh.write(version)

    print("post_version_txt: %s -> %r" % (out_path, version))


# Run after firmware.bin is linked (same folder for http.server OTA test)
bin_path = os.path.join(env.subst("$BUILD_DIR"), "firmware.bin")
env.AddPostAction(bin_path, write_version_txt)
