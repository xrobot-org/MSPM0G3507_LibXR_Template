#!/bin/sh
# Fetch the files of the pinned MSPM0 SDK commit that the build needs (source/ and the
# startup file) into mspm0-sdk/, instead of cloning the whole SDK submodule.

set -eu

cd "$(dirname "$0")/.."

url=$(git config -f .gitmodules submodule.mspm0-sdk.url)
commit=$(git rev-parse HEAD:mspm0-sdk)

mkdir -p mspm0-sdk
cd mspm0-sdk

if [ ! -e .git ]; then
    git init -q .
fi
if ! git remote get-url origin >/dev/null 2>&1; then
    git remote add origin "$url"
fi

git sparse-checkout init --no-cone
git sparse-checkout set \
    '/source/' \
    '/examples/nortos/LP_MSPM0G3507/driverlib/empty_driverlib_library/gcc/'

git fetch --depth 1 --filter=blob:none origin "$commit"
git checkout -q FETCH_HEAD

echo "MSPM0 SDK $commit is in $(pwd)"
