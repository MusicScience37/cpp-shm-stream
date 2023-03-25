import pathlib

import pytest


def pytest_addoption(parser: pytest.Parser) -> None:
    parser.addoption(
        "--build", action="store", required=True, help="Build directory of CMake."
    )


@pytest.fixture
def build_dir(pytestconfig: pytest.Config) -> pathlib.Path:
    return pathlib.Path(str(pytestconfig.getoption("build"))).absolute()


@pytest.fixture
def bin_dir(build_dir: pathlib.Path) -> pathlib.Path:
    return build_dir / "bin"


@pytest.fixture
def blocking_writer(bin_dir: pathlib.Path) -> pathlib.Path:
    return bin_dir / "test_integ_blocking_writer"


@pytest.fixture
def blocking_reader(bin_dir: pathlib.Path) -> pathlib.Path:
    return bin_dir / "test_integ_blocking_reader"
