# Project ownership boundaries

`src/helios_ui/` is maintained externally and copied into this repository.
Treat the entire tree as an imported dependency, including custom code and
documentation. Do not edit it here unless the user explicitly requests a
change to that imported tree.

Implement firmware integration and callback overrides outside that tree,
normally under `src/core/`. Put project-specific documentation under `docs/`.
If a requested feature requires an upstream UI change, describe the required
change for the external UI project instead of patching this copy.

Preserve existing user changes in the imported tree.
