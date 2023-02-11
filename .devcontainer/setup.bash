#!/bin/bash

git config --global --add safe.directory $(pwd)

poetry config virtualenvs.in-project true
poetry env use 3.10
poetry install

poetry run conan profile new --force --detect default
poetry run conan profile update settings.compiler.libcxx=libc++ default

poetry run pre-commit install

git config commit.template .gitmessage

git config gpg.program gpg2
git config commit.gpgsign true
git config tag.gpgsign true

echo "source /usr/share/bash-completion/completions/git" >>~/.bashrc