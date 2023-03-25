"""Benchmark of sending an receiving data."""

import pathlib
import subprocess
import sys


def bench(build_dir: pathlib.Path) -> None:
    server_process = subprocess.Popen(
        [str(build_dir / "bin" / "bench_ping_pong_server")]
    )
    try:
        client_result = subprocess.run(
            [str(build_dir / "bin" / "bench_ping_pong_client")],
            check=False,
        )
    finally:
        server_process.terminate()
        server_process.wait(timeout=5)

    assert server_process.returncode == 0
    assert client_result.returncode == 0


if __name__ == "__main__":
    bench(pathlib.Path(sys.argv[1]))
