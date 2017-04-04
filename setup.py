
from setuptools import setup, find_packages

setup(
    name='dellve',
    version='0.1.2',
    packages=find_packages(),
    include_package_data=True,
    package_data={
        'dellve': [
            'data/jinja2/*',
            'data/config.yaml'
        ]
    },
    install_requires=[
        'click',
        'daemonocle',
        'falcon',
        'gevent',
        'jinja2',
        'pick',
        'pyyaml',
        'stringcase',
        'tqdm'
    ],
    setup_requires=[
        'pytest-runner'
    ],
    tests_require=[
        'py>=1.4.30',
        'pytest>=3.0.3'
    ],
    entry_points='''
        [console_scripts]
        dellve=dellve.cli:cli
    '''
)
