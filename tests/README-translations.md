# Translation Source Regression Test

Run in an MSVC developer shell with CMake, Ninja and Qt Linguist tools available:

```powershell
python tests/test_translation_sources.py
```

The default Windows SDKs are Qt 5.15.16 and Qt 6.9.1 under `C:/Qt`.
Override their `qtbase` paths with `LIMEREPORT_TEST_QT5` and
`LIMEREPORT_TEST_QT6` for other installations.

The test copies the real source tree into a temporary directory, configures the
real CMake project and runs only its translation target. It covers all four
combinations of barcode and dialog-designer support for each Qt version. Source
files for disabled features must not enter compilation, but must remain in the
translation scan. All six catalogs must retain their messages and translations,
and all six QM files must be produced. Original TS files are never rewritten.
No application or full C++ library build is started.
