from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
import subprocess
import os

class CMakeExtension(Extension):
    def __init__(self, name):
        super().__init__(name, sources=[])

class BuildCMakeExt(build_ext):
    def run(self):
        for ext in self.extensions:
            self.build_cmake(ext)
        super().run()

    def build_cmake(self, ext):
        if not os.path.exists('build'):
            os.makedirs('build')
        os.chdir('build')
        subprocess.check_call(['cmake', '..'])
        subprocess.check_call(['cmake', '--build', '.'])
        os.chdir('..')


setup(
    name="fzzpy",
    version="0.1",
    packages=find_packages(),
    ext_modules=[CMakeExtension("fzzpy")],
    cmdclass={"build_ext": BuildCMakeExt},
    install_requires=[],
    author="平岡研究室区間近似実用化チーム",
    author_email="xu.chenguang.k34@kyoto-u.jp",
    description="Python wrapper for FastZigzag computation from fzz C++ library.",
    long_description=open('README.md').read(),
    long_description_content_type="text/markdown",
    url="https://github.com/Commutative-Ladder/fzzpy",  # Replace with your repo URL
)
