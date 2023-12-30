#! /usr/bin/env bash
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
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
# Install and Run dismod_at in a Docker Image
# ###########################################
#
# Purpose
# *******
# This bash script will create a dismod_at and at_cascade OCI image.
# Using this script is an  alternative to going through the steps required to
# :ref:`install_dismod_at<install_unix-name>` .
# You can use the following link to get a get a copy of the dismod_at.sh
# `bash script <https://raw.githubusercontent.com/bradbell/dismod_at/master/bin/dock_dismod_at.sh>`_
# If you understand docker, this script also serves as an example
# install of dismod_at.
#
# Syntax
# ******
#
# Build Image
# ===========
#
# | ``./dock_dismod_at.sh image base``
# | ``./dock_dismod_at.sh image mixed``
# | ``./dock_dismod_at.sh image dismod_at``
# | ``./dock_dismod_at.sh image at_cascade``
#
# Run Container
# =============
#
# | *driver* ``run -it --name`` *container_name* ``dismod_at.image bash``
# | *driver* ``run -it --name`` *container_name* ``at_cascade.image bash``
#
# Resume Container
# ================
#
# | *driver* ``start`` *container_name*
# | *driver* ``exec -it`` *container_name* ``bash``
#
# Copy Files
# ==========
#
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
#
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
#     ``./dock_dismod_at.sh image base >&`` *log_file*
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
# Building Images
# ***************
# The image commands above will not execute if the corresponding OCI image
# already exists.
# You must remove containers that use an image and then remove the image,
# before you can execute the image command successfully.
#
# dismod_at.base
# ==============
# The ``image base`` syntax creates a new OCI image with the name
# ``dismod_at.base`` .
# The :ref:`whats_new<whats_new_2019-name>` instructions will tell you if
# you need to re-execute this command.
#
# dismod_at.mixed
# ===============
# The ``image mixed`` syntax creates a new OCI image with the name
# ``dismod_at.mixed`` .
# The ``dismod_at.base`` image must exist before the
# ``dismod_at.mixed`` image can be created.
# The :ref:`whats_new<whats_new_2019-name>` instructions will tell you if
# you need to re-execute this command.
#
# dismod_at.image
# ===============
# The ``image dismod_at`` syntax creates a new OCI image with the name
# ``dismod_at.image`` .
# The ``dismod_at.mixed`` image must exist before the
# ``dismod_at.image`` image can be created.
#
# at_cascade.image
# ================
# The ``image at_cascade`` syntax creates a new OCI image with the name
# ``at_cascade.image`` .
# The ``dismod_at.image`` image must exist before the
# ``at_cascade.image`` image can be created.
# The `at_cascade <https://bradbell.github.io/at_cascade/doc/rst/at_cascade.html>`_ package is an optional add-on to the dismod_at program.
#
# Dockerfile
# ==========
# The :ref:`dock_dismod_at.sh@Syntax@Build Image` commands above will create a
# `Dockerfile <https://docs.docker.com/glossary/?term=Dockerfile>`_
# in the current working directory.
# If such a file already exists, it will need to be moved or deleted.
#
# dismod_at_version
# *****************
# This script will build the following version of dismod_at image:
# {xrst_spell_off}
# {xrst_code sh}
   dismod_at_version='20231229'
   dismod_at_hash='06d27afd568be551762fc9b4d5f63cd5566de8cf'
# {xrst_code}
# {xrst_spell_on}
#
# at_cascade_version
# ******************
# This script can build the following version of the optional at_cascade image:
# {xrst_spell_off}
# {xrst_code sh}
   at_cascade_version='2023.11.30'
   at_cascade_hash='76ed17ae2a3bcd4977d8c22cecc25b9bbd3ad283'
# {xrst_code}
# {xrst_spell_on}
#
# {xrst_end dock_dismod_at.sh}
# ---------------------------------------------------------------------------
ok='true'
if [ "$#" != 2 ]
then
   ok='false'
elif [ "$1" != 'image' ]
then
   ok='false'
elif [ "$2" != 'base' ] \
&& [ "$2" != 'mixed' ] \
&& [ "$2" != 'dismod_at' ] \
&& [ "$2" != 'at_cascade' ]
then
   ok='false'
fi
if [ "$ok" != 'true' ]
then
   echo 'usage: dock_dismod_at.sh image base'
   echo 'usage: dock_dismod_at.sh image mixed'
   echo 'usage: dock_dismod_at.sh image dismod_at'
   echo 'usage: dock_dismod_at.sh image at_cascade'
   exit 1
fi
# ---------------------------------------------------------------------------
if ! $driver ps > /dev/null
then
   echo "Cannot run $dirver ps"
   if [ "$dirver" == 'docker' ]
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
if [ "$2" == 'base' ]
then
   image_name="dismod_at.base"
elif [ "$2" == 'mixed' ]
then
   image_name="dismod_at.mixed.$build_type"
elif [ "$2" == 'dismod_at' ]
then
   image_name="dismod_at.image.$build_type"
elif [ "$2" == 'at_cascade' ]
then
   image_name="at_cascade.image.$build_type"
else
   'dock_dismod_at.sh: program error'
   exit 1
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
   echo 'Must first remove following OCI containers:'
   $driver ps -a | head -1
   $driver ps -a | grep "$image_name"
   echo 'Use the following command for each container_id above:'
   echo "$driver rm contain_id"
   exit 1
fi
if $driver images | grep "$image_name " > /dev/null
then
   echo 'dock_dismod_at.sh Error'
   echo 'Must first remove following images:'
   $driver images | head -1
   $driver images | grep "$image_name "
   echo 'Use the following command for each image above:'
   echo "$driver rmi image_id"
   exit 1
fi
echo 'Creating Dockerfile'
#
# prefix
prefix='/home/venv'
# ----------------------------------------------------------------------------
if [ "$2" == 'base' ]
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
# pip packages
RUN pip3 install matplotlib numpy scipy build
#
# 1. Get source corresponding to dismod_at-$dismod_at_version
# 2. Check that the corresponding hash is $dismod_at_hash
# 3. Change some settings in bin/run_cmake.sh
RUN \
git clone https://github.com/bradbell/dismod_at.git dismod_at.git && \
cd dismod_at.git && \
git checkout --quiet $dismod_at_hash  && \
grep "$dismod_at_version" CMakeLists.txt > /dev/null && \
sed -i bin/run_cmake.sh \
   -e 's|^dismod_at_prefix=.*|dismod_at_prefix='$prefix'|' \
   -e "s|^build_type=.*|build_type='$build_type'|" \
   -e "s|^python3_executable=.*|python3_executable='$prefix/bin/python3'|"
EOF
# ----------------------------------------------------------------------------
elif [ "$2" == 'mixed' ]
then
cat << EOF > Dockerfile
FROM dismod_at.base
WORKDIR /home/dismod_at.git
RUN bin/get_cppad_mixed.sh
EOF
# -----------------------------------------------------------------------------
elif [ "$2" == 'dismod_at' ]
then
site_packages="$prefix/lib/python3.11/site-packages"
cat << EOF > Dockerfile
FROM dismod_at.mixed.$build_type
WORKDIR /home/dismod_at.git

# LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH=''

# PATH
# must escape PATH variable so it gets interpreted in the image
ENV PATH="\$PATH:$prefix/bin"

# Build, check, install, check install location
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
elif [ "$2" == 'at_cascade' ]
then
cat << EOF > Dockerfile
FROM dismod_at.image.$build_type
WORKDIR /home

# 1. Get source corresponding to at_cascade hash
# 2. Check the corresponding at_cascade version
# 3. Remove building the documentaiton from check_all.sh
RUN \
git clone https://github.com/bradbell/at_cascade.git at_cascade.git && \
cd at_cascade.git && \
git checkout --quiet $at_cascade_hash && \
grep "at_cascade-$at_cascade_version\$" at_cascade.xrst > /dev/null && \
sed -i bin/check_all.sh -e '/run_xrst.sh/d' && \
sed -i bin/run_test.sh -e 's|if python3|if $prefix/bin/python3|'

WORKDIR /home/at_cascade.git

# Test at_cascade
RUN bin/check_all.sh

# Install at_cascade
RUN \
$prefix/bin/python3 -m build && \
pip3 install --force-reinstall dist/at_cascade-$at_cascade_version.tar.gz
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
