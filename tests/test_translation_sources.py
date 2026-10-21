"""Exercise real CMake/lupdate/lrelease with optional features on and off.

Run from a compiler developer shell. Override Qt SDK prefixes with
LIMEREPORT_TEST_QT5 and LIMEREPORT_TEST_QT6 when needed.
"""
import itertools
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest
import xml.etree.ElementTree as ET


ROOT = Path(__file__).resolve().parents[1]
LANGUAGES = ("ar", "es", "fr", "pl", "ru", "zh")
CMAKE = shutil.which("cmake") or "cmake"
NINJA = shutil.which("ninja") or (
    "C:/Program Files/Microsoft Visual Studio/2022/Community/"
    "Common7/IDE/CommonExtensions/Microsoft/CMake/Ninja/ninja.exe")


def translations(path):
    result = {}
    for context in ET.parse(path).getroot().findall("context"):
        for message in context.findall("message"):
            key = (context.findtext("name"), message.findtext("source"), message.findtext("comment"))
            value = message.find("translation")
            value.tail = None
            result[key] = ET.tostring(value, encoding="unicode")
    return result


class TranslationSourcesTest(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.temporary = tempfile.TemporaryDirectory(prefix="limereport translations ")
        cls.root = Path(cls.temporary.name).resolve()
        cls.source = cls.root / "source"
        cls.source.mkdir()
        shutil.copy2(ROOT / "CMakeLists.txt", cls.source)
        for name in ("cmake", "3rdparty", "limereport", "translations", "include"):
            shutil.copytree(ROOT / name, cls.source / name,
                            ignore=shutil.ignore_patterns(".git", "build", "x64", "release", "debug",
                                                          "*.obj", "*.pdb", "*.lib", "*.dll", "*.qm"))
        cls.baselines = {lang: (cls.source / f"translations/limereport_{lang}.ts").read_bytes()
                         for lang in LANGUAGES}
        cls.project = cls.root / "project"
        cls.project.mkdir()
        (cls.project / "CMakeLists.txt").write_text(
            'cmake_minimum_required(VERSION 3.20)\nproject(TranslationCheck LANGUAGES C CXX)\n'
            'find_package(Qt${TEST_QT_MAJOR} REQUIRED COMPONENTS Core LinguistTools)\n'
            'set(QT_VERSION "${Qt${TEST_QT_MAJOR}Core_VERSION}")\n'
            'if(NOT QT_VERSION)\n'
            '  set(QT_VERSION "${Qt${TEST_QT_MAJOR}Core_VERSION_STRING}")\nendif()\n'
            f'add_subdirectory("{cls.source.as_posix()}" limereport)\n'
            'get_target_property(compiled limereport-qt${TEST_QT_MAJOR} SOURCES)\n'
            f'get_directory_property(scanned DIRECTORY "{cls.source.as_posix()}" '
            'DEFINITION LIMEREPORT_TRANSLATION_SOURCES)\n'
            'file(WRITE "${CMAKE_BINARY_DIR}/compiled.txt" "${compiled}")\n'
            'file(WRITE "${CMAKE_BINARY_DIR}/scanned.txt" "${scanned}")\n', encoding="utf-8")

    @classmethod
    def tearDownClass(cls):
        if not cls.root.is_relative_to(Path(tempfile.gettempdir()).resolve()):
            raise RuntimeError("Unsafe temporary output directory")
        cls.temporary.cleanup()

    def command(self, args, log, env):
        result = subprocess.run(args, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                                env=env, timeout=180)
        log.write_bytes(result.stdout)
        self.assertEqual(result.returncode, 0, result.stdout.decode(errors="replace")[-7000:])

    def check_qt(self, major):
        default = f"C:/Qt/{'5.15.16' if major == 5 else '6.9.1'}.x64/qtbase"
        prefix = Path(os.environ.get(f"LIMEREPORT_TEST_QT{major}", default))
        if not prefix.is_dir():
            self.skipTest(f"Qt {major} SDK not found: {prefix}")
        expected_sources = None
        for barcode, designer in itertools.product((False, True), repeat=2):
            with self.subTest(qt=major, barcode=barcode, designer=designer):
                for lang, data in self.baselines.items():
                    (self.source / f"translations/limereport_{lang}.ts").write_bytes(data)
                original = {lang: translations(self.source / f"translations/limereport_{lang}.ts")
                            for lang in LANGUAGES}
                build = self.root / f"qt{major}-barcode{int(barcode)}-designer{int(designer)}"
                build.mkdir()
                env = dict(os.environ, PATH=str(prefix / "bin") + os.pathsep + os.environ["PATH"])
                self.command([CMAKE, "-G", "Ninja", "-S", str(self.project), "-B", str(build),
                              f"-DCMAKE_MAKE_PROGRAM={NINJA}", f"-DCMAKE_PREFIX_PATH={prefix}",
                              f"-DTEST_QT_MAJOR={major}", f"-DUSE_QT6={'ON' if major == 6 else 'OFF'}",
                              f"-DENABLE_ZINT={'ON' if barcode else 'OFF'}",
                              f"-DENABLE_DIALOG_DESIGNER={'ON' if designer else 'OFF'}",
                              "-DHAVE_UI_LOADER=ON", "-DUPDATE_TRANSLATIONS=ON", "-DLIMEREPORT_STATIC=OFF"],
                             build / "configure.log", env)
                compiled = set((build / "compiled.txt").read_text().split(";"))
                scanned = set((build / "scanned.txt").read_text().split(";"))
                self.assertEqual("limereport/items/lrbarcodeitem.cpp" in compiled, barcode)
                self.assertEqual("limereport/dialogdesigner/lrdialogdesigner.cpp" in compiled, designer)
                self.assertIn("limereport/items/lrbarcodeitem.cpp", scanned)
                self.assertIn("limereport/dialogdesigner/lrdialogdesigner.cpp", scanned)
                self.assertIn("limereport/lrfactoryinitializer.cpp", scanned)
                if expected_sources is not None:
                    self.assertEqual(scanned, expected_sources)
                expected_sources = scanned
                self.command([CMAKE, "--build", str(build), "--target", f"limereport-qt{major}_lrelease"],
                             build / "translations.log", env)
                for lang in LANGUAGES:
                    updated = translations(self.source / f"translations/limereport_{lang}.ts")
                    self.assertEqual(updated, original[lang], f"Qt {major}: translations changed for {lang}")
                    self.assertIn(("QObject", "Barcode Item", None), updated)
                qm_files = list(build.rglob("limereport_*.qm"))
                self.assertEqual(len(qm_files), len(LANGUAGES))
                self.assertTrue(all(path.stat().st_size > 1000 for path in qm_files))
                print(f"PASS: Qt {major}, barcode={barcode}, designer={designer}; six catalogs preserved", flush=True)

    def test_qt5_optional_features(self):
        self.check_qt(5)

    def test_qt6_optional_features(self):
        self.check_qt(6)


if __name__ == "__main__":
    unittest.main(verbosity=2)
