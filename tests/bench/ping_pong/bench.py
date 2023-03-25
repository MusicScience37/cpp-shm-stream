"""Benchmark of sending an receiving data."""

import pathlib
import subprocess
import sys
import time


def bench(build_dir: pathlib.Path) -> None:
    bench_results_dir = build_dir / "bench" / "bench_ping_pong"
    server_process = subprocess.Popen(
        [str(build_dir / "bin" / "bench_ping_pong_server")]
    )
    time.sleep(1)

    try:
        client_result = subprocess.run(
            [
                str(build_dir / "bin" / "bench_ping_pong_client"),
                "--plot",
                str(bench_results_dir),
                "--json",
                str(bench_results_dir / "result.json"),
            ],
            check=False,
        )
    finally:
        server_process.terminate()
        server_process.wait(timeout=5)

    assert server_process.returncode == 0
    assert client_result.returncode == 0


if __name__ == "__main__":
    bench(pathlib.Path(sys.argv[1]).absolute())
