#build_type! /usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-24 Bradley M. Bell
# ----------------------------------------------------------------------------
#
# exit on error or undefined variable
set -e -u
#
# {xrst_begin dock_dismod_at.sh}
# {xrst_spell
#     busybox
#     cp
#     dockerfile
#     get get
#     podman
#     rm
#     rmi
# }
# {xrst_comment_ch #}
#
# Install and Run dismod_at and at_cascade in Docker Images
# #########################################################
#
# Purpose
# *******
# This bash script will create the dismod_at OCI images.
# Using this script is an alternative to going through the steps required to
# :ref:`install_dismod_at<install_unix-name>` and
# `at_cascade <https://at-cascade.readthedocs.io>`_ .
# You can use the following link to get a get a copy of
# `dock_dock_dismod_at/sh <https://raw.githubusercontent.com/bradbell/dismod_at/master/bin/dock_dismod_at.sh>`_
# If you understand docker, this script also serves as an example
# install of dismod_at.
#
# Syntax
# ******
#
#  .. csv-table::
#     :widths: auto
#     :header: Command, image_name
#
#     ``./dock_dismod_at.sh base``      ,``dismod_at.base``
#     ``./dock_dismod_at.sh mixed``     ,``dismod_at.mixed.``\ *build_type*
#     ``./dock_dismod_at.sh dismod_at`` ,``dismod_at.dismod_at.``\ *build_type*
#     ``./dock_dismod_at.sh at_cascade``,``dismod_at.at_cascade.``\ *build_type*
#
# #.  The *build_type* can be ``debug`` or ``release`` ; see
#     :ref:`dock_dismod_at.sh@build_type` .
# #.  Each image above depends on its previous image; e.g., if you
#     rebuild ``dismod_at.base`` , you must rebuild all the other images.
#     The :ref:`release_notes-name` will mention when new versions
#     of the images are available.
# #.  The image commands above will not execute if the corresponding image
#     already exists.
#     You must remove containers that use an image and then remove the image,
#     before you can create a new version of the image.
# #.  The commands above will create a
#     `Dockerfile <https://docs.docker.com/glossary/?term=Dockerfile>`_
#     in the current working directory.
#     If such a file already exists, it will need to be moved or deleted
#     before the command can be executed.
#
# Create A Container
# ==================
# | *driver* ``run -it --name`` *container_name* *image_name* ``bash``
#
# Resume Container
# ================
# | *driver* ``start`` *container_name*
# | *driver* ``exec -it`` *container_name* ``bash``
#
# Copy Files
# ==========
# | *driver* ``cp`` *file_name* |space| *container_name*\ ``:/home/work``
# | *driver* ``cp`` *container_name*\ ``:/home/work/``\ *file_name* |space| .
#
# Remove Container
# ================
# | *driver* stop *container_name*
# | *driver* rm *container_name*
#
#
# Remove Image
# ============
# | *driver* rmi *image_name*
#
#
# driver
# ******
# The *driver* program, determined by this setting, can be either
# ``docker`` or ``podman`` :
# {xrst_spell_off}
# {xrst_code sh}
   driver='podman'
# {xrst_code}
# {xrst_spell_on}
# Above and below we refer to the value of this shell variable as *driver* .
# Both docker and podman create Open Container Initiative
# `OCI <https://opencontainers.org/>`_ images and containers.
#
# build_type
# **********
# The *build_type* , determined by this setting, can be either
# ``debug`` or ``release`` :
# The debug version is much slower but does much more error checking.
# {xrst_code sh}
   build_type='release'
# {xrst_code}
# The same base image, ``dismod_at.base`` can be used for both
# debug and release builds.
# All the other images names depend on the build type and have the build type
# at the end of the name.
#
# Logging Output
# **************
# You can save the output of any of the commands above by redirecting
# standard output and standard error to a file.
# For example,
#
#     ``./dock_dismod_at.sh base >&`` *log_file*
#
# will redirect standard output and standard error to *log_file* .
# If you do this, you will not see the progress during execution.
# If also want to monitor the progress, in another window use
#
#     ``tail -f`` *log_file*
#
# This ``tail`` command will not terminate until you enter
# control-C in the window where it is running.
#
# Requirements
# ************
# You must have a copy of `docker <https://docs.docker.com/>`_, or
# `podman <https://podman.io>`_
# installed on your system.
# You can test this on your system by trying to execute the following command:
#
#     *driver* ``run busybox echo`` ``'Hello World'``
#
# dismod_at_version
# *****************
# This script can build the following version of ``dismod_at.dismod_at``
# {xrst_spell_off}
# {xrst_code sh}
   dismod_at_version='2024.8.13'
   dismod_at_hash='1f637c2941ab6b0501eed9ac65011bc826675e16'
# {xrst_code}
# {xrst_spell_on}
#
# at_cascade_version
# ******************
# This script can build the following version of ``dismod_at.at_cascade``
# {xrst_spell_off}
# {xrst_code sh}
   at_cascade_version='2024.7.30'
   at_cascade_hash='346a37ccec4b95d505afe895aa4c63c68374bf46'
# {xrst_code}
# {xrst_spell_on}
#
# {xrst_end dock_dismod_at.sh}
# ---------------------------------------------------------------------------
ok='true'
if [ "$#" != 1 ]
then
   ok='false'
elif [ "$1" != 'base' ] \
&& [ "$1" != 'mixed' ] \
&& [ "$1" != 'dismod_at' ] \
&& [ "$1" != 'at_cascade' ]
then
   ok='false'
fi
if [ "$ok" != 'true' ]
then
   echo 'usage: dock_dismod_at.sh base'
   echo 'usage: dock_dismod_at.sh mixed'
   echo 'usage: dock_dismod_at.sh dismod_at'
   echo 'usage: dock_dismod_at.sh at_cascade'
   exit 1
fi
# ---------------------------------------------------------------------------
if ! $driver ps > /dev/null
then
   echo "Cannot run $driver ps"
   if [ "$driver" == 'docker' ]
   then
cat << EOF
If docker deamon is not running perhaps one of the following will start it:
   sudo systemctl start docker
   sudo service docker start
If it is a premission problem perhaps one of the following will get permission:
   sudo groupadd docker
   sudo usermod -aG docker $USER
then log out and long back on this system.
EOF
   fi
   exit 1
fi
# ---------------------------------------------------------------------------
# Build images
# ----------------------------------------------------------------------------
if [ "$1" == 'base' ]
then
   image_name="dismod_at.base"
else
   image_name="dismod_at.$1.$build_type"
fi
if [ -e 'Dockerfile' ]
then
   echo 'dock_dismod_at.sh Error'
   echo "Must first remove ./Dockerfile"
   exit 1
fi
if $driver ps -a | grep "$image_name" > /dev/null
then
   echo 'dock_dismod_at.sh Error'
   echo 'Must first remove following containers:'
   $driver ps -a | head -1
   $driver ps -a | grep "$image_name"
   echo 'Use the following command for each container_id above:'
   echo "$driver rm contain_id"
   exit 1
fi
if $driver images | grep "$image_name " > /dev/null
then
   echo 'dock_dismod_at.sh Error'
   echo 'Must first remove following image:'
   $driver images | head -1
   $driver images | grep "$image_name "
   echo 'Use the following command to remove the image above:'
   echo "$driver rmi image_id"
   exit 1
fi
echo 'Creating Dockerfile'
#
# prefix
prefix='/home/venv'
# ----------------------------------------------------------------------------
if [ "$1" == 'base' ]
then
cat << EOF > Dockerfile
# -----------------------------------------------------------------------------
# Ubuntu 23.04 with dismod_at requirements that are installed using apt-get.
# The vim editor is included for use when debugging containers and
# is not required by dismod_at.
# -----------------------------------------------------------------------------
FROM ubuntu:23.04
RUN  apt-get update
WORKDIR /home
#
# apt-get packages
RUN  \
DEBIAN_FRONTEND=noninteractive apt-get install -y  \
cmake \
git \
gfortran \
libblas-dev \
liblapack-dev \
libsuitesparse-dev \
libgsl-dev \
libsqlite3-dev \
pkg-config \
python3 \
python3-venv \
vim \
wget
#
# activate-python-virtualenv-in-dockerfile
# https://stackoverflow.com/questions/48561981
RUN python3 -m venv $prefix
ENV VIRTUAL_ENV     $prefix
ENV PATH            $prefix/bin:\$PATH
#
# pip packages required by dismod_at
RUN pip3 install matplotlib numpy scipy
#
EOF
# ----------------------------------------------------------------------------
elif [ "$1" == 'mixed' ]
then
cat << EOF > Dockerfile
FROM dismod_at.base
WORKDIR /home
#
# Get source corresponding to dismod_at-$dismod_at_version
RUN git clone https://github.com/bradbell/dismod_at.git dismod_at.git
#
# WORKDIR
WORKDIR /home/dismod_at.git
#
RUN \
git checkout --quiet $dismod_at_hash  && \
grep "$dismod_at_version" CMakeLists.txt > /dev/null
#
# Change bin/install_settings.py
RUN sed -i bin/install_settings.py \
   -e "s|^dismod_at_prefix *=.*|dismod_at_prefix = '$prefix'|" \
   -e "s|^build_type *=.*|build_type = '$build_type'|"  \
   -e "s|^python3_executable *=.*|python3_executable = '$prefix/bin/python3'|"
#
# Install cppad_mixed
RUN bin/get_cppad_mixed.sh
EOF
# -----------------------------------------------------------------------------
elif [ "$1" == 'dismod_at' ]
then
site_packages="$prefix/lib/python3.11/site-packages"
cat << EOF > Dockerfile
FROM dismod_at.mixed.$build_type
WORKDIR /home/dismod_at.git
#
# Check soruce
RUN \
mv bin/install_settings.py temp.py && \
git pull origin master && \
git checkout --quiet $dismod_at_hash  && \
mv temp.py bin/install_settings.py && \
grep "$dismod_at_version" CMakeLists.txt > /dev/null && \
grep "^build_type *= *'$build_type'" bin/install_settings.py> /dev/null
#
# LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH=''
#
# PATH
# must escape PATH variable so it gets interpreted in the image
ENV PATH="\$PATH:$prefix/bin"
#
# dismod_at: Build, check, install, check install location
RUN \
bin/run_cmake.sh && \
cd build && \
make -j \$(nproc) && \
make check && \
make install && \
make install_python && \
ls "$site_packages/dismod_at" > /dev/null && \
cd ..

WORKDIR /home/work
EOF
# ----------------------------------------------------------------------------
elif [ "$1" == 'at_cascade' ]
then
cat << EOF > Dockerfile
FROM dismod_at.dismod_at.$build_type
WORKDIR /home
#
# Check dismod_at source
RUN \
cd dismod_at.git && \
mv bin/install_settings.py temp.py && \
git pull origin master && \
git checkout --quiet $dismod_at_hash  && \
mv temp.py bin/install_settings.py && \
grep "$dismod_at_version" CMakeLists.txt > /dev/null && \
grep "^build_type *= *'$build_type'" bin/install_settings.py> /dev/null
#
# Get at_cascade source
RUN git clone https://github.com/bradbell/at_cascade.git at_cascade.git
#
# WORKDIR
WORKDIR /home/at_cascade.git
#
# Get at_cascade corresponding to hash
RUN \
git checkout --quiet $at_cascade_hash && \
grep "at_cascade-$at_cascade_version\$" at_cascade.xrst > /dev/null && \
sed -i bin/run_test.sh -e 's|if python3|if $prefix/bin/python3|'
#
# Test at_cascade
RUN bin/check_all.sh
#
# Install at_cascade
RUN pip3 install .
#
WORKDIR /home/work
EOF
# ----------------------------------------------------------------------------
else
   echo 'dock_dismod_at.sh: program error'
   exit 1
fi
#
echo "Creating $image_name"
$driver build --tag $image_name .
#
echo "dock_dismod_at.sh: OK: created $driver image: $image_name"
exit 0
