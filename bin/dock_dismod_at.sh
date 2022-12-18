#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin dock_dismod_at.sh}
# {xrst_spell
#     busybox
#     dockerfile
#     get get
#     podman
#     ps
#     rm
#     rmi
# }
# {xrst_comment_ch #}
#
# Install and Run dismod_at in a Docker Image
# ###########################################
#
# Syntax
# ******
#
# | ./ ``dock_dismod_at.sh image base``
# | ./ ``dock_dismod_at.sh image mixed``
# | ./ ``dock_dismod_at.sh image dismod_at``
# | ./ ``dock_dismod_at.sh image at_cascade``
# | ./ ``dock_dismod_at.sh`` *build_type* *database* *command*  ...
#
# OCI
# ***
# Both docker and podman create Open Container Initiative
# `OCI <https://opencontainers.org/>`_ images and containers.
#
# driver
# ======
# The *driver* program, determined by this setting, can be either
# docker or podman:
# {xrst_spell_off}
# {xrst_code sh}
   driver='podman'
# {xrst_code}
# {xrst_spell_on}
# Below we referee to the value of this shell variable as *driver* .
#
# Logging Output
# **************
# You can save the output of any of the commands above by redirecting
# standard output and standard error to a file.
# For example,
#
#     ./ ``dock_dismod_at.sh image base >&`` *log_file*
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
# Purpose
# *******
# This bash script will create or run a dismod_at OCI image
# and can be run from any directory.
# Using this script is an  alternative to going through the steps required to
# :ref:`install_dismod_at<install_unix-name>` .
# You can use the following link to get a get a copy of the dismod_at.sh
# `bash script <https://raw.githubusercontent.com/bradbell/dismod_at/master/bin/dock_dismod_at.sh>`_
# If you understand docker, this script also serves as an example
# install of dismod_at.
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
# The image commands will not execute if the corresponding OCI image
# already exists.
# You must remove containers that use an image and then remove the image,
# before you can execute the image command successfully.
#
# dismod_at_version
# =================
# This script will build the following version of dismod_at image:
# {xrst_spell_off}
# {xrst_code sh}
   dismod_at_version='20221218'
   dismod_at_hash='288c27e763e636675ca68ba5a302e6aab3f41b3c'
# {xrst_code}
# {xrst_spell_on}
#
# at_cascade_version
# ==================
# This script can build the following version of the optional at_cascade image:
# {xrst_spell_off}
# {xrst_code sh}
   at_cascade_version='2022.12.18'
   at_cascade_hash='77904c5e24f8c7054da1ba86cd000d640aef4c66'
# {xrst_code}
# {xrst_spell_on}
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
# Removing Containers
# ===================
# If an existing container uses an image that is being created,
# you will be prompted with the corresponding *container_id* .
# The command
#
#     *driver* ``rm`` *container_id*
#
# will remove the container.
# If the container is still running, you will need to use
#
#     *driver* ``rm --force`` *container_id*
#
# Removing Images
# ===============
# You can remove an old image using the command
#
#     *driver* ``rmi`` *name*
#
# For example, *name* could be
# ``dismod_at.base`` ,
# ``dismod_at.mixed`` , or
# ``dismod_at.image`` .
# You can keep the old image, under a different name, using the commands
#
# | |tab| *driver* ``tag`` *name* *different_name*
# | |tab| *driver* ``rmi`` *name*
#
# Dockerfile
# ==========
# The ``build`` syntax will create the file
# `Dockerfile <https://docs.docker.com/glossary/?term=Dockerfile>`_
# in the current working directory.
# If such a file already exists, it will need to be moved or deleted.
#
# Errors
# ======
#
# #. If you get the error message
#
#        ``Unable to fetch some archives`` , ``maybe run apt-get update`` ...
#
#    There may be an old OCI image result for ``apt-get update``
#    that is out of date.
#    You can list the images using the command ``OCI images`` .
#    Try removing an old image that corresponds to a previous
#    ``apt-get update`` and then re-run the
#    ``dock_dismod_at.sh build`` command.
#
# #. If you get the error message
#
#        ``Release file for`` *package* ``is not valid yet`` ...
#
#    You system clock may be out of date (reporting an old day or time).
#    Try fixing the system clock.
#
# Run New Container
# *****************
# Once ``dismod_at.image`` has been created, you use the
# *build_type* syntax to run dismod_at in a container.
#
# Removing Containers
# ===================
# The dismod_at container for a particular *user* will be named
# ``dismod_at.`` *user* .
# If such a container already exists,
# you will be prompted with the corresponding *container_id* .
# The command
#
#     *driver* ``rm`` *container_id*
#
# will remove the container.
# If the container is still running, you will need to use
#
#     *driver* ``rm --force`` *container_id*
#
# build_type
# ==========
# The *build_type* syntax will run the correspond
# :ref:`command-name` in the OCI image.
# The argument *build_type* must be either ``debug`` or
# ``release`` .
# The ``release`` version should be much faster.
# The ``debug`` version will do more extensive error checking.
#
# database
# ========
# The second argument *database* must be a dismod_at database
# in the current working directory.
# This the first argument to the corresponding dismod_at command.
#
# command
# =======
# The third argument *command* must be one of the dismod_at commands.
# This is the second argument in the corresponding dismod_at command.
# The rest of the arguments ... are
# the same as the corresponding arguments for the :ref:`command-name` .
#
# Other Arguments
# ===============
# The other arguments to ``dock_dismod_at.sh`` are the same as in the
# syntax for the :ref:`command-name` ,
# except that ``dismod_at`` or ``dismodat.py``
# have been replaced by ``dock_dismod_at.sh`` *build_type* .
#
# Debugging
# *********
# Some times an error occurs during the running of a container
# and you would like to go inside the container and execute commands.
# The following instructions are useful for this:
#
# Determine Container Id
# ======================
#
#     *driver* ``ps -a``
#
# If *driver* is podman, the following might work better
#
#     ``podman ps --all --storage``
#
# Start Container
# ===============
# If a container status is ``Exited`` , you can start it using:
#
#     ``driver`` *start* ``container_id``
#
# Run Container
# =============
# If a container status is ``Up`` , you can run it using:
#
#     *driver* ``exec -it`` *container_id* ``bash``
#
# You will be in the container until you ``exit``
# the ``bash`` shell that is run by the command above.
#
# Stop Container
# ==============
# If a container status is ``Up`` , you can stop it using:
#
#     ``driver`` *stop* ``container_id``
#
# {xrst_end dock_dismod_at.sh}
# ---------------------------------------------------------------------------
if [ "$1" == 'image' ]
then
   if [ "$2" != 'base' ] \
   && [ "$2" != 'mixed' ] \
   && [ "$2" != 'dismod_at' ] \
   && [ "$2" != 'at_cascade' ]
   then
      echo 'usage: dock_dismod_at.sh image base'
      echo 'usage: dock_dismod_at.sh image mixed'
      echo 'usage: dock_dismod_at.sh image dismod_at'
      echo 'usage: dock_dismod_at.sh image at_cascade'
      echo 'usage: dock_dismod_at.sh debug database command ...'
      echo 'usage: dock_dismod_at.sh release database command ...'
      exit 1
   fi
else
   if [ "$1" != 'debug' ] && [ "$1" != 'release' ]
   then
      echo 'usage: dock_dismod_at.sh image base'
      echo 'usage: dock_dismod_at.sh image mixed'
      echo 'usage: dock_dismod_at.sh image dismod_at'
      echo 'usage: dock_dismod_at.sh image at_cascade'
      echo 'usage: dock_dismod_at.sh debug   database command ...'
      echo 'usage: dock_dismod_at.sh release database command ...'
      exit 1
   fi
fi
# ---------------------------------------------------------------------------
if ! $driver ps > /dev/null
then
cat << EOF
Cannot run docker ps
If docker deamon is not running perhaps one of the following will start it:
   sudo systemctl start docker
   sudo service docker start
If it is a premission problem perhaps one of the following will get permission:
   sudo groupadd docker
   sudo usermod -aG docker $USER
then log out and long back on this system.
EOF
   exit 1
fi
# ---------------------------------------------------------------------------
# Build images
# ----------------------------------------------------------------------------
if [ "$1" == 'image' ]
then
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
# ----------------------------------------------------------------------------
elif [ "$image_name" == 'dismod_at.image' ]
then
cat << EOF > Dockerfile
# -----------------------------------------------------------------------------
FROM dismod_at.mixed
WORKDIR /home/dismod_at.git

# LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH=''

# 1. Get source corresponding to dismod_at-$dismod_at_version
# 2. Check that the corresponding hash is $dismod_at_hash
# 3. Change install prefix to /home/prefix/dismod_at
RUN git checkout master && \
git pull && \
git checkout --quiet $dismod_at_hash  && \
sed -i bin/run_cmake.sh -e 's|\$HOME/|/home/|g' && \
grep "$dismod_at_version" CMakeLists.txt > /dev/null

# Install debug version of dismod_at
RUN sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='debug'|" && \
bin/run_cmake.sh && \
cd build && \
make check && \
make install install_python && \
cd ..

# Install release version of dismod_at
RUN sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='release'|" && \
bin/run_cmake.sh && \
cd build && \
make check && \
make install install_python && \
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

# LD_LIBRARY_PATH
ENV LD_LIBRARY_PATH=''

# 1. Get source corresponding to at_cascasde-$at_cascade_version
WORKDIR /home
RUN git clone https://github.com/bradbell/at_cascade.git at_cascade.git && \
cd at_cascade.git && \
git checkout --quiet $at_cascade_hash && \
grep "Version $at_cascade_version\$" at_cascade.xrst > /dev/null && \
sed -i bin/check_all.sh -e '/run_xrst.sh/d'

RUN if [ ! -e $dir/dismod_at.release ] ; \
then echo 'Cannot find $dir/dismod_at.release' ; exit 1; fi
RUN if [ -e $dir/dismod_at ] ; then rm $dir/dismod_at ; fi && \
ln -s $dir/dismod_at.release $dir/dismod_at

# 2. Test debug
WORKDIR /home/at_cascade.git
RUN \
export PATH="\$PATH:$dir/dismod_at/bin" && \
export PYTHONPATH=\$(find -L $dir/dismod_at -name site-packages) && \
if [ -e $dir/dismod_at ] ; then rm $dir/dismod_at ; fi && \
ln -s $dir/dismod_at.debug $dir/dismod_at && \
bin/check_all.sh

# 3. Install debug
RUN python3 -m build && \
pip3 install --force-reinstall dist/at_cascade-$at_cascade_version.tar.gz \
   --prefix=$dir/dismod_at

# 4. Test release
WORKDIR /home/at_cascade.git
RUN \
export PATH="\$PATH:$dir/dismod_at/bin" && \
export PYTHONPATH=\$(find -L $dir/dismod_at -name site-packages) && \
if [ -e $dir/dismod_at ] ; then rm $dir/dismod_at ; fi && \
ln -s $dir/dismod_at.release $dir/dismod_at && \
bin/check_all.sh

# 5. Install release
RUN python3 -m build && \
pip3 install --force-reinstall dist/at_cascade-$at_cascade_version.tar.gz \
   --prefix=$dir/dismod_at
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
fi
# ---------------------------------------------------------------------------
# Run dismod_at in Docker container
# ----------------------------------------------------------------------------
build_type="$1"
database="$2"
cmd="$3"
shift # so that $* are the argument database command ...
# ----------------------------------------------------------------------------
# check cmd
dismod_at_cmd='init fit set depend sample predict old2new'
dismodat_py_cmd='db2csv csv2db modify'
program=''
for check in $dismod_at_cmd
do
   if [ "$check" == "$cmd" ]
   then
      program='dismod_at'
   fi
done
for check in $dismodat_py_cmd
do
   if [ "$check" == "$cmd" ]
   then
      program='dismodat.py'
   fi
done
if [ "$program" == '' ]
then
   echo 'dock_dismod_at.sh Error'
   echo "$cmd is not a valid dismod_at command"
   exit 1
fi
# ----------------------------------------------------------------------------
if [ ! -f "$database" ]
then
   echo 'dock_dismod_at.sh Error'
   echo "cannot find the database $database"
   exit 1
fi
container_name="dismod_at.$USER"
# check that the previous dismod_at container has been deleted
if $driver ps -a | grep " $container_name\$" > /dev/null
then
   echo 'dock_dismod_at.sh Error'
   echo 'Must first remove following containers:'
   $driver ps -a | head -1
   $driver ps -a | grep " $container_name\$"
   echo 'Use the following command for each container_id above:'
   echo "$driver rm contain_id"
   exit 1
fi
#
# create a new dismod_at container
echo "echo 'exit 0' | \\"
echo "$driver run -i --name $container_name dismod_at.image bash"
echo 'exit 0' | $driver run -i --name "$container_name" dismod_at.image bash
#
# get the id for the new container
container_id=`$driver ps -a -q --filter "name=$container_name"`
#
# copy the database to the container
echo "$driver cp $database $container_id:/home/work/$database"
$driver cp "$database"  "$container_id:/home/work/$database"
#
# create a temporary directory
temporary_dir=`mktemp`
rm $temporary_dir
mkdir $temporary_dir
#
# copy work.sh to the container
cat << EOF > "$temporary_dir/work.sh"
#! /bin/bash -e
#
# prefix for install
prefix='/home/prefix/dismod_at'
#
# Add to executable path
PATH="\$prefix/bin:\$PATH"
#
# Add to python path
export PYTHONPATH=\`find -L \$prefix -name site-packages\`
#
# Set debug or release
if [ -e \$prefix ]
then
   rm \$prefix
fi
ln -s \$prefix.$build_type \$prefix
#
# execute the dismod_at command
$program $*
#
# done
exit 0
EOF
chmod +x $temporary_dir/work.sh
echo "$driver cp $temporary_dir/work.sh $container_id:/home/work/work.sh"
$driver cp "$temporary_dir/work.sh" "$container_id:/home/work/work.sh"
#
# start up the container
echo "$driver start $container_id"
$driver start $container_id
#
# execute work.sh
echo "$driver exec -it $container_id ./work.sh"
$driver exec -it $container_id ./work.sh
#
# copy the result back to temporary directory
echo "$driver cp $container_id:/home/work $temporary_dir/work"
$driver cp $container_id:/home/work "$temporary_dir/work"
#
# remove the container
echo "$driver rm --force $container_id"
$driver rm --force $container_id
#
# copy the results from temporary directory to working directory
for file in `ls $temporary_dir/work/*`
do
   name=`echo $file | sed -e 's|.*/||'`
   if [ "$name" != 'work.sh' ]
   then
      echo "cp $file $name"
      cp $file $name
   fi
done
#
# remove the temporary directory
echo "rm -r $temporary_dir"
rm -r $temporary_dir
# ----------------------------------------------------------------------------
echo 'dock_dismod_at.sh: OK'
exit 0
