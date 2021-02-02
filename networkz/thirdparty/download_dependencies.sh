#!/usr/bin/env bash

# This script downloads all the thirdparty dependencies as a series of tarballs
# that can be used for offline builds, etc.

set -eu

SOURCE_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

if [ "$#" -ne 1 ]; then
  DESTDIR=$(pwd)
else
  DESTDIR=$1
fi

# DESTDIR=$(readlink -f "${DESTDIR}")

download_dependency() {
  local url=$1
  local out=$2

  wget --quiet --continue --output-document="${out}" "${url}" || \
    (echo "Failed downloading ${url}" 1>&2; exit 1)
}

main() {
  mkdir -p "${DESTDIR}"

  # Load `DEPENDENCIES` variable.
  source ${SOURCE_DIR}/versions.txt

  echo "# Environment variables for offline build"
  for ((i = 0; i < ${#DEPENDENCIES[@]}; i++)); do
    local dep_packed=${DEPENDENCIES[$i]}

    # Unpack each entry of the form "$home_var $tar_out $dep_url"
    IFS=" " read -r dep_url_var dep_tar_name dep_url <<< "${dep_packed}"

    local out=${DESTDIR}/${dep_tar_name}
    echo "Download from ${dep_url}"
    download_dependency "${dep_url}" "${out}"

    echo "export ${dep_url_var}=${out}"
  done
}

main
