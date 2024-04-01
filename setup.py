from skbuild_conan import setup
from setuptools import find_packages

setup(  # https://scikit-build.readthedocs.io/en/latest/usage.html#setup-options
    name="qtg",
    version="0.1.0",
    packages=find_packages("pysrc"),  # Include all packages in `./src`.
    package_dir={"": "pysrc"},  # The root for our python package is in `./src`.
    python_requires=">=3.7",  # lowest python version supported.
    install_requires=[""],  # Python Dependencies
    conan_requirements=["gsl/[>=2.6]", "gmp/[>=6.2.1]"],  # C++ Dependencies
    #conan_profile="x86",
    cmake_minimum_required_version="3.23",
)