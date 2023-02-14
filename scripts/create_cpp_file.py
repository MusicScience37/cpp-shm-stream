#!/usr/bin/env python3
"""Create a file of source codes in C++.
"""

# MIT License
#
# Copyright (c) 2020 Kenta Kabashima
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import datetime
import pathlib
import typing

import click


HEADER = R"""/*
 * Copyright {year} MusicScience37 (Kenta Kabashima)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
/*!
 * \file
 * \brief {brief}.
 */
"""

PRAGMA_ONCE = R"""#pragma once
"""

ENCODING = "utf8"


def current_year() -> int:
    """Get the current year.

    Returns:
        int: Current year.
    """
    now = datetime.datetime.now()
    return now.year


def create_cpp_file(
    filepath: typing.Union[str, pathlib.Path],
    *,
    brief: typing.Optional[str] = None,
    overwrite: bool = False,
    additional_contents: typing.Optional[str] = None,
):
    """Create a file of source codes in C++.

    Args:
        filepath (typing.Union[str, pathlib.Path]) : Filepath.
        overwrite (bool) : Overwrite if exists.
        additional_contents: Additional contents of the file.
    """
    if isinstance(filepath, str):
        filepath = pathlib.Path(filepath)
    filepath = filepath.absolute()

    if not brief:
        brief = filepath.stem

    if filepath.exists() and not overwrite:
        raise FileExistsError(f"File already exists: {filepath}")

    default_contents = HEADER.format(
        year=current_year(),
        brief=brief,
    )
    if filepath.suffix == ".h":
        default_contents = default_contents + PRAGMA_ONCE
    if additional_contents is not None:
        default_contents = default_contents + additional_contents

    filepath.write_text(default_contents, encoding=ENCODING)


@click.command()
@click.argument("filepath", nargs=1)
def main(filepath: str):
    """Create a file of source codes in C++."""
    create_cpp_file(filepath)


if __name__ == "__main__":
    main()
