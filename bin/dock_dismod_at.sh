#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin dock_dismod_at.sh}
# {xrst_spell
#     busybox
#     dockerfile
#     get get
#     podman
#     rm
#     rmi
#     cp
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
# docker or podman:
# {xrst_spell_off}
# {xrst_code sh}
   driver='podman'
# {xrst_code}
# {xrst_spell_on}
# Above and below we refer to the value of this shell variable as *driver* .
# Both docker and podman create Open Container Initiative
# `OCI <https://opencontainers.org/>`_ images and containers.
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
   dismod_at_version='20230628'
   dismod_at_hash='7b88bac7895f0c49b3586e5676fc00e0a41aeff6'
# {xrst_code}
# {xrst_spell_on}
#
# at_cascade_version
# ******************
# This script can build the following version of the optional at_cascade image:
# {xrst_spell_off}
# {xrst_code sh}
   at_cascade_version='2023.6.28'
   at_cascade_hash='98c53cf11c1dd6da44762866dbba34862a7d8613'
# {xrst_code}
# {xrst_spell_on}
#
# {xrst_end dock_dismod_at.sh}
# ---------------------------------------------------------------------------
ok='true'
if [ "$1" != 'image' ]
then
   ok='false'
fi
if [ "$2" != 'base' ] \
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
   image_name='dismod_at.base'
elif [ "$2" == 'mixed' ]
then
   image_name='dismod_at.mixed'
elif [ "$2" == 'dismod_at' ]
then
   image_name='dismod_at.image'
elif [ "$2" == 'at_cascade' ]
then
   image_name='at_cascade.image'
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
# ----------------------------------------------------------------------------
if [ "$image_name" == 'dismod_at.base' ]
then
cat << EOF > Dockerfile
# -----------------------------------------------------------------------------
# Ubuntu 20.04 with dismod_at requirements that are installed using apt-get.
# The vim editor is included for use when debugging containers and
# is not required by dismod_at.
# -----------------------------------------------------------------------------
FROM ubuntu:20.04
RUN  apt-get update
RUN  DEBIAN_FRONTEND=noninteractive apt-get install -y  \
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
pip \
vim \
wget
#
Run pip3 install \
matplotlib \
numpy \
scipy \
build

# 1. Get dismod git repository as /home/dismod_at.git
WORKDIR /home
RUN git clone https://github.com/bradbell/dismod_at.git dismod_at.git
EOF
# ----------------------------------------------------------------------------
elif [ "$image_name" == 'dismod_at.mixed' ]
then
cat << EOF > Dockerfile
FROM dismod_at.base
WORKDIR /home/dismod_at.git

# 1. Get source corresponding to dismod_at-$dismod_at_version
# 2. Check that the corresponding hash is $dismod_at_hash
# 3. Change install prefix to /home/prefix/dismod_at
# 4. Get cppad_mixed library and dependencies

RUN git pull && \
git checkout --quiet $dismod_at_hash  && \
grep "$dismod_at_version" CMakeLists.txt > /dev/null && \
mkdir /home/prefix && \
sed -i bin/run_cmake.sh -e 's|\$HOME/|/home/|g'

# debug install debug version of eigen and ipopt
RUN sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='debug'|" && \
bin/get_cppad_mixed.sh

# release install release version of eigen and ipopt
RUN sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='release'|" && \
bin/get_cppad_mixed.sh

# Restore run_cmake.sh to its original state
Run git checkout bin/run_cmake.sh
EOF
elif [ "$image_name" == 'dismod_at.image' ]
then
dir='/home/prefix'
site_packages="$dir/dismod_at/lib/python3.8/site-packages"
cat << EOF > Dockerfile
# -----------------------------------------------------------------------------
FROM dismod_at.mixed
WORKDIR /home/dismod_at.git

# LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH=''

# PYTHONPATH
ENV PYTHONPATH="$site_packages"

# PATH
# must escape PATH variable so it gets interpreted in the image
ENV PATH="\$PATH:$dir/dismod_at/bin"

# 1. Get source corresponding to dismod_at hash
# 2. Check the corresponding dismod_at version
# 3. Change install prefix to /home/prefix/dismod_at
RUN git checkout master && \
git pull && \
git checkout --quiet $dismod_at_hash  && \
grep "$dismod_at_version" CMakeLists.txt > /dev/null &&\
sed -i bin/run_cmake.sh -e 's|\$HOME/|/home/|g'

# 1. Change build_type to debug
# 2. Build, check, and install debug version
# 3. Check install location
RUN sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='debug'|" && \
bin/run_cmake.sh && \
cd build && \
make check && \
make install install_python && \
ls "$site_packages/dismod_at" > /dev/null && \
cd ..

# 1. Change build_type to release
# 2. Build, check, and install release version
# 3. Check install location
RUN sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='release'|" && \
bin/run_cmake.sh && \
cd build && \
make check && \
make install install_python && \
ls "$site_packages/dismod_at" > /dev/null && \
cd ..

# Restore run_cmake.sh to its original state
Run git checkout bin/run_cmake.sh

WORKDIR /home/work
EOF
# ----------------------------------------------------------------------------
elif [ "$image_name" == 'at_cascade.image' ]
then
dir='/home/prefix'
cat << EOF > Dockerfile
FROM dismod_at.image
WORKDIR /home

# 1. Get source corresponding to at_cascade hash
# 2. Check the corresponding at_cascade version
# 3. Remove building the documentaiton from check_all.sh
RUN git clone https://github.com/bradbell/at_cascade.git at_cascade.git && \
cd at_cascade.git && \
git checkout --quiet $at_cascade_hash && \
grep "at_cascade-$at_cascade_version\$" at_cascade.xrst > /dev/null && \
sed -i bin/check_all.sh -e '/check_xrst.sh/d'

# Test using debug version of dismod_at
WORKDIR /home/at_cascade.git
RUN \
if [ -e $dir/dismod_at ] ; then rm $dir/dismod_at ; fi && \
ln -s $dir/dismod_at.debug $dir/dismod_at && \
echo "skip debug version of bin/check_all.sh"

# Install in /home/prefix/dismod_at.debug
RUN python3 -m build && \
pip3 install --force-reinstall dist/at_cascade-$at_cascade_version.tar.gz \
   --prefix=$dir/dismod_at

# Test using release version of dismod_at
WORKDIR /home/at_cascade.git
RUN \
if [ -e $dir/dismod_at ] ; then rm $dir/dismod_at ; fi && \
ln -s $dir/dismod_at.release $dir/dismod_at && \
bin/check_all.sh

# Install in /home/prefix/dismod_at.release
RUN python3 -m build && \
pip3 install --force-reinstall dist/at_cascade-$at_cascade_version.tar.gz \
   --prefix=$dir/dismod_at
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
echo "dock_dismod_at.sh $1 $2: OK"
exit 0
