# Security policy

Until 1.0, only the latest tagged release and the current `main` branch receive
security fixes.

Please use GitHub private vulnerability reporting. Do not open a public issue
for a suspected vulnerability. Include the affected version, Windows version,
MSVC toolset, a minimal reproducer, and the expected impact. Maintainers will
acknowledge a report within seven days and coordinate disclosure after a fix is
available.

Native handles, callback lifetime, cross-thread access, malformed shell data,
and dependency integrity are security-relevant boundaries.
