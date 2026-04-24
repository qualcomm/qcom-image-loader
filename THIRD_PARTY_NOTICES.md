# Third-Party Notices

`qil` (Qualcomm Image Loader) is distributed under the
[BSD-3-Clause Clear License](LICENSE.txt) and incorporates or
statically links the following third-party components. Each
component is covered by its own license; those licenses remain in
effect for the respective component and do **not** apply to `qil`'s
own source code.

Copies of the referenced licenses are in the [`LICENSES/`](LICENSES/)
folder.

---

## libusb (1.0.x)

- Upstream project : <https://libusb.info>
- Upstream source  : <https://github.com/libusb/libusb>
- Version linked   : **1.0.27**
- Exact source     : <https://github.com/libusb/libusb/releases/tag/v1.0.27>
- Modifications    : none (the upstream release is used unmodified)
- License          : **GNU Lesser General Public License v2.1 or later**
                     (LGPL-2.1-or-later). See
                     [`LICENSES/libusb-1.0-LGPL-2.1.txt`](LICENSES/libusb-1.0-LGPL-2.1.txt).
- Link mode        : `qil` currently links libusb **statically**
                     into `qil.exe` / `libQcDevice.a` on Windows and
                     Linux. See
                     [`doc/LGPL-COMPLIANCE.md`](doc/LGPL-COMPLIANCE.md)
                     for how static linking is handled for an
                     open-source consumer and what downstream
                     redistributors of `qil.exe` need to do.

> Notice required by LGPL §6: *This product uses libusb, a library
> licensed under the GNU Lesser General Public License version 2.1
> or (at your option) any later version. A copy of the license is
> included in this distribution. The corresponding source code of
> the libusb version used by this product is available from
> <https://github.com/libusb/libusb/releases/tag/v1.0.27>.*

---

## libxml2 (Windows build only)

- Upstream project : <https://gitlab.gnome.org/GNOME/libxml2>
- Link mode        : statically linked into Windows builds via the
                     prebuilt archives under
                     `src/external/libxml2-win-build/`.
- License          : MIT. Upstream `Copyright.txt` is embedded in
                     the `libxml2-win-build` tree; see
                     `src/external/libxml2-win-build/`.

## libiconv (Windows build only)

- Upstream project : <https://www.gnu.org/software/libiconv/>
- Link mode        : statically linked into Windows builds via the
                     prebuilt archives under
                     `src/external/libiconv-win-build/`.
- License          : See
                     `src/external/libiconv-win-build/COPYING` and
                     `src/external/libiconv-win-build/LICENSE.md`.

## zlib / liblzma (Windows build only)

- Link mode        : statically linked into Windows builds via
                     archives shipped by the `libxml2-win-build`
                     sub-tree.
- Licenses         : zlib License (permissive) and 0BSD /
                     public-domain for liblzma. Authoritative
                     copies live with the respective upstream
                     projects.

## kLogger

- Upstream project : vendored under `src/external/kLogger/`.
- License          : see `src/external/kLogger/LICENSE`.

---

## Adding a new third-party dependency?

1. Drop the verbatim upstream license text into `LICENSES/<name>-<SPDX-id>.txt`.
2. Add an entry to this file with: name, upstream URL, version,
   license SPDX identifier, link mode (static / dynamic), and a
   pointer to the license copy.
3. If the new component is under a copyleft license (LGPL, GPL,
   MPL, …), add the corresponding §6 / source-availability
   treatment to `doc/LGPL-COMPLIANCE.md`.