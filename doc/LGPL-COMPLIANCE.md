# LGPL Compliance Guide for qil

`qil` is licensed under the **BSD 3-Clause Clear License**, but it
statically links against **libusb-1.0**, which is licensed under
the **GNU Lesser General Public License v2.1 or later**
(LGPL-2.1-or-later). Static linking triggers specific LGPL
obligations on anyone who **distributes a qil binary**.

This document captures:

1. What the LGPL obligations are and *why* they apply here.
2. How the qil project itself satisfies them.
3. What **downstream redistributors** (anyone shipping `qil.exe` or
   the `qil` Linux binary to end users) have to do on top.
4. How to remove most of these obligations (switch to dynamic
   linking — see issue/PR tracker).

This is general engineering guidance; it is not legal advice.
If you are redistributing qil in a commercial product, have your
legal team review the compliance bundle before shipping.

---

## 1. What LGPL §6 actually requires

The relevant text is LGPL-2.1 §6. A work that *uses* the Library —
i.e., a binary that links against libusb, whether statically or
dynamically — is called a "work that uses the Library" and may be
distributed under these conditions:

| Obligation | Source | Applies when distributing… |
|---|---|---|
| A. Give prominent notice that the work uses the Library and is covered by LGPL. | §6 para. 1 | a binary that links libusb (any link mode) |
| B. Accompany the binary with the LGPL-2.1 license text. | §6 para. 1 | a binary that contains libusb bytes (static or dynamic) |
| C. Provide access to the *corresponding* libusb source (exact version, including any modifications, under LGPL). | §6 para. 1 + §4 | a binary that contains libusb bytes |
| D. Enable the recipient to **relink** the combined work against a modified libusb. This is the core clause. | §6(a) and §6(b) | a binary that contains libusb bytes |
| E. Do not obstruct the recipient's ability to substitute a modified libusb (no signature/version pinning). | §6(b) + §6 para. 4 | any binary that links libusb dynamically |

§6(a) lets you satisfy D by shipping the complete object files of
the non-libusb part of the work (your code), plus any scripts /
data needed to re-run the linker.

§6(b) lets you satisfy D by using a "shared library mechanism" —
dynamic linking — so the user can drop in a new `libusb-1.0.dll` /
`libusb-1.0.so` without relinking.

**Key consequence for static linking**: if you statically link
libusb, you must satisfy D via §6(a). For a closed-source
distributor, that means shipping object files of their own code —
a major burden. For an open-source distributor (like qil) the
same requirement is automatically satisfied by the existence of
the public source repository: a user can clone qil, swap libusb,
and rebuild. The obligation doesn't disappear; its cost is
effectively zero.

---

## 2. How qil itself complies (today, with static linking)

| Obligation | How qil handles it |
|---|---|
| A. Prominent notice | `README.md` has a "Third-party components" section pointing at `THIRD_PARTY_NOTICES.md`. Every binary release (GitHub Releases asset) includes both files. |
| B. LGPL text | `LICENSES/libusb-1.0-LGPL-2.1.txt` is in the repo and bundled into every binary release by the release workflow. |
| C. libusb source access | `THIRD_PARTY_NOTICES.md` gives the exact libusb release tag. The version is unmodified, so the upstream tag URL satisfies §6 §4. If qil ever patches libusb, those patches must be placed under `src/qds/libusb/patches/` and the notice file updated. |
| D. Relink ability (§6(a)) | Auto-satisfied: qil's source is public, licensed under BSD-3-Clause-Clear, and a contributor can rebuild qil against a modified libusb simply by swapping the pinned version in `src/qds/libusb/` and re-running `cmake --build`. The `doc/User_Guide.md` explains the build steps. |
| E. No obstruction | qil does not check any signature/hash of `libusb-1.0.dll` at runtime. |

### What qil ships in every release

Every release artifact (installer, zip, tarball) must contain:

```
<artifact root>/
├── qil.exe (or qil)
├── LICENSE.txt                         (qil's own BSD-3-Clause-Clear)
├── THIRD_PARTY_NOTICES.md
└── LICENSES/
    └── libusb-1.0-LGPL-2.1.txt
```

If the release also ships `libusb-1.0.dll` (which it does **not**
in the normal qil release flow — the DLL comes from the driver
installer or from `apt install libusb-1.0-0` on Linux), the
release is *also* the distributor of libusb bits and must re-copy
the same three files next to the DLL.

---

## 3. What **downstream redistributors** of qil binaries must do

Anyone who takes `qil.exe` (or the Linux binary) and ships it as
part of a larger product is redistributing a work that statically
contains libusb. They inherit obligations A–D above with respect
to *their* distribution. Concretely, any downstream product that
bundles qil must:

1. **Include the LGPL-2.1 license text** in the final product
   (typically under `Licenses/` inside the installer or in the
   "About" / "Acknowledgements" dialog).
2. **Include an attribution notice** stating that the product
   contains qil, which statically links libusb under
   LGPL-2.1-or-later. A single paragraph is enough.
3. **Provide access to the exact libusb source** (link to the
   upstream release tag used by qil; include any patches that
   exist in `src/qds/libusb/patches/`).
4. **Provide a way for the user to rebuild qil against a modified
   libusb.** The simplest way is to point the user at the public
   qil repository (<https://github.qualcomm.com/ProdTools/qil>) —
   that is §6(a) satisfied through public source access. If the
   downstream distribution is itself open source or the
   downstream product does not obstruct source access, nothing
   more is needed.
5. **Pass obligations further down the chain.** Anyone who takes
   that downstream product and redistributes it inherits the same
   list.

### Example passthrough notice for a downstream product

```
This product contains qil (Qualcomm Image Loader),
https://github.com/qualcomm/qil, which is licensed under the
BSD 3-Clause Clear License and statically links libusb
(https://libusb.info), licensed under the GNU Lesser General
Public License version 2.1 or (at your option) any later
version.

Copies of both licenses are included in this distribution under
"Licenses/". The corresponding source code of the libusb version
linked into qil is available from
https://github.com/libusb/libusb/releases/tag/v1.0.27 . The qil
source code is available from
https://github.com/qualcomm/qil .

The user may rebuild qil against a modified libusb by following
the build instructions in the qil repository.
```

---

## 4. How to reduce compliance work: switch to dynamic linking

If qil is ever consumed by **closed-source** downstream products,
static linking forces those products to satisfy §6(a) — ship
their own object files. They cannot ride on qil's open-source
pass-through because their own code is proprietary.

Switching qil to dynamic linking:

- Shifts the §6(a) burden to §6(b), which is satisfied by simply
  shipping a replaceable `libusb-1.0.dll` / `libusb-1.0.so`.
- Lets every downstream consumer (open- or closed-source) comply
  with a one-line notice + the DLL copy, no object files ever.
- Is already the default on Linux (via `pkg-config`) and the
  intended path on Windows — see the PR that introduces
  `src/qds/libusb/lib-1.0.27/dll/<Platform>/libusb-1.0.lib`.

This document will be updated when qil switches to dynamic
linking on Windows. At that point obligation D moves from §6(a)
(source-access) to §6(b) (DLL-replaceability) and most of §3 can
be simplified.

---

## 5. Change management

- Bumping the vendored libusb version
  - Update the tag / URL in `THIRD_PARTY_NOTICES.md`.
  - Re-fetch the upstream `COPYING` and replace
    `LICENSES/libusb-1.0-LGPL-2.1.txt` with the new verbatim copy.
  - Update any release-notes boilerplate that quotes the version.
- Applying a local patch to libusb
  - Put the patch under `src/qds/libusb/patches/NNNN-<description>.patch`.
  - Add a row to `THIRD_PARTY_NOTICES.md` listing the patch and
    its stated LGPL-2.1 licensing.
  - Ensure the release bundle continues to include the libusb
    source *or* a pointer plus the patch set.
- Removing libusb as a dependency
  - Delete the entries above when it actually leaves the tree.

---

## 6. Not legal advice

The above captures the project maintainers' best-effort
understanding of what LGPL-2.1 §6 requires of an open-source
consumer that statically links libusb. It does not replace review
by your legal team when shipping commercial products.