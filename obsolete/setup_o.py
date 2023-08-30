from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
import subprocess
import os
from distutils.command.build import build as _build

__version__ = "0.1.0"

class BuildCMake(_build):
    sub_commands = _build.sub_commands + [('build_ext', None)]
    
    def run(self):
        self.run_command('build_ext')
        _build.run(self)

class BuildCMakeExt(build_ext):
    def run(self):
        self.build_cmake()
        super().run()

    def build_cmake(self):
        if not os.path.exists('build'):
            os.makedirs('build')
        os.chdir('build')
        subprocess.check_call(['cmake', '..'])
        subprocess.check_call(['cmake', '--build', '.'])
        os.chdir('..')


setup(
    name="fzzpy",
    version=__version__,
    packages=find_packages(),
    # ext_modules=[CMakeExtension("fzzpy")],
    cmdclass={"build_ext": BuildCMakeExt, "build": BuildCMake},
    install_requires=[],
    author="平岡研究室区間近似実用化チーム",
    author_email="xu.chenguang.k34@kyoto-u.jp",
    description="Python wrapper for FastZigzag computation from fzz C++ library.",
    long_description=open('README.md').read(),
    long_description_content_type="text/markdown",
    url="https://github.com/Commutative-Ladder/fzzpy",  # Replace with your repo URL
)
