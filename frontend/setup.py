"""HeteroSA: AutoSA fork targeting TAPA.

See:
https://github.com/dotkrnl/heterosa
"""

import re

from os import path

from setuptools import find_packages, setup

here = path.abspath(path.dirname(__file__))

with open(path.join(here, '..', 'CMakeLists.txt'), encoding='utf-8') as f:
  version = '.'.join(x[1] for x in re.finditer(
      r'set\(CPACK_PACKAGE_VERSION_..... (.*)\)', f.read()))

setup(
    name='heterosa',
    version=version,
    description='AutoSA fork targeting TAPA',
    url='https://github.com/dotkrnl/heterosa',
    author='Jason Lau, Jie Wang',
    classifiers=[
        'Development Status :: 2 - Pre-Alpha',
        'Intended Audience :: Developers',
        'Intended Audience :: Science/Research',
        'License :: OSI Approved :: MIT License',
        'Programming Language :: Python :: 3 :: Only',
        'Programming Language :: Python :: 3',
        'Programming Language :: Python :: 3.6',
        'Programming Language :: Python :: 3.7',
        'Programming Language :: Python :: 3.8',
        'Programming Language :: Python :: 3.9',
        'Topic :: System :: Hardware',
    ],
    packages=find_packages(),
    python_requires='>=3.6',
    install_requires=[
        'decorator>=4.3.0',
        'joblib>=0.14.1',
        'numpy>=1.18.2',
        'pandas>=1.0.3',
        'scipy>=1.4.1',
        'sklearn>=0.0',
        'sympy>=1.4',
        'xgboost>=0.81',
    ],
    entry_points={
        'console_scripts': [
            'heterosa=heterosa.cli:main',
            'heterosa_codegen=heterosa.codegen:main',
            'heterosa_opt=heterosa.optimizer:main',
        ],
    },
    include_package_data=True,
)