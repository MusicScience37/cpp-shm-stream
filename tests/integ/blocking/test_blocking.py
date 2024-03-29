"""Test of streams of bytes without waiting."""

import concurrent.futures
import pathlib
import subprocess
import time


def test_send_data(
    blocking_writer: pathlib.Path, blocking_reader: pathlib.Path
) -> None:
    executor = concurrent.futures.ThreadPoolExecutor()
    reader_process = subprocess.Popen(
        [str(blocking_reader)], stdout=subprocess.PIPE, encoding="ascii"
    )
    try:
        reader_future = executor.submit(lambda: reader_process.communicate(timeout=5))
        time.sleep(0.1)

        writer_process = subprocess.Popen(
            [str(blocking_writer)], stdin=subprocess.PIPE, encoding="ascii"
        )
        data = "abc\n" + "abcdefgh\n" + "abcdefghijklmnopqrstuvwxyz\n"
        writer_process.communicate(input=data + "\n", timeout=5)
        writer_process.wait(timeout=5)
    finally:
        reader_process.terminate()
        reader_process.wait(timeout=5)

    assert writer_process.returncode == 0
    assert reader_process.returncode == 0

    stdout, _ = reader_future.result(timeout=5)
    assert stdout == data
