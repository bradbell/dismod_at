#! /bin/bash -e
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-22 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin dock_dismod_at.sh$$ $newlinech #$$
# $spell
#	dismod
#	busybox
#	rmi
#	Dockerfile
#	dismodat.py
#	rm
#	ps
#   podman
# $$
#
# $section Install and Run dismod_at in a Docker Image$$
#
# $head Syntax$$
# $codei%./dock_dismod_at.sh image base
# %$$
# $codei%./dock_dismod_at.sh image mixed
# %$$
# $codei%./dock_dismod_at.sh image dismod_at
# %$$
# $codei%./dock_dismod_at.sh %build_type% %database% %command% %...%
# %$$
#
# $head OCI$$
# Both docker and podman create Open Container Initiative
# $href%https://opencontainers.org/%OCI%$$ images and containers.
#
# $subhead driver$$
# The $icode driver$$ program, determined by this setting, can be either
# docker or podman:
# $srccode%sh%
	driver='podman'
# %$$
# Below we referee to the value of this shell variable as $icode driver$$.
#
# $head Logging Output$$
# You can save the output of any of the commands above by redirecting
# standard output and standard error to a file.
# For example,
# $codei%
#	./dock_dismod_at.sh image base >& %log_file%
# %$$
# will redirect standard output and standard error to $icode log_file$$.
# If you do this, you will not see the progress during execution.
# If also want to monitor the progress, in another window use
# $codei%
#	tail -f %log_file%
# %$$
# This $code tail$$ command will not terminate until you enter
# control-C in the window where it is running.
#
# $head Purpose$$
# This bash script will create or run a dismod_at OCI image
# and can be run from any directory.
# Using this script is an  alternative to going through the steps required to
# $cref/install_dismod_at/install_unix/$$.
# You can get a copy of this script at the following link
# $href%https://raw.githubusercontent.com/bradbell/dismod_at/master/bin/dock_dismod_at.sh
#	%dock_dismod_at.sh
# %$$
# If you understand docker, this script also serves as an example
# install of dismod_at.
#
# $head Requirements$$
# You must have a copy of $href%https://docs.docker.com/%docker%$$, or
# $href%https://podman.io%podman%$$
# installed on your system.
# You can test this on your system by trying to execute the following command:
# $codei%
#	%driver% run busybox echo 'Hello World'
# %$$
#
# $head Building Images$$
# The image commands will not execute if the corresponding OCI image
# already exists.
# You must remove containers that use an image and then remove the image,
# before you can execute the image command successfully.
#
# $subhead dismod_at_version$$
# This script will build the following version of dismod_at image:
# $srccode%sh%
	dismod_at_version='20220421'
	dismod_at_hash='b96c2d107a712537646b41eefaa6dbc5e817f0f3'
# %$$
#
# $subhead at_cascade_version$$
# This script can build the following version of the optional at_cascade image:
# $srccode%sh%
    at_cascade_version='2022.4.18'
    at_cascade_hash='92412aef5c04201c2351ee68c3cef8f02a2fbcad'
# %$$
#
#
# $subhead dismod_at.base$$
# The $code image base$$ syntax creates a new OCI image with the name
# $code dismod_at.base$$.
# The $cref/whats_new/whats_new_2019/$$ instructions will tell you if
# you need to re-execute this command.
#
# $subhead dismod_at.mixed$$
# The $code image mixed$$ syntax creates a new OCI image with the name
# $code dismod_at.mixed$$.
# The $code dismod_at.base$$ image must exist before the
# $code dismod_at.mixed$$ image can be created.
# The $cref/whats_new/whats_new_2019/$$ instructions will tell you if
# you need to re-execute this command.
#
# $subhead dismod_at.image$$
# The $code image dismod_at$$ syntax creates a new OCI image with the name
# $code dismod_at.image$$.
# The $code dismod_at.mixed$$ image must exist before the
# $code dismod_at.image$$ image can be created.
#
# $subhead at_cascade.image$$
# The $code image at_cascade$$ syntax creates a new OCI image with the name
# $code at_cascade.image$$.
# The $code dismod_at.image$$ image must exist before the
# $code at_cascade.image$$ image can be created.
# The $href%
#   https://bradbell.github.io/at_cascade/doc/xsrst/at_cascade.html%
#   at_cascade
# %$$ package is an optional add-on to the dismod_at program.
#
# $subhead Removing Containers$$
# If an existing container uses an image that is being created,
# you will be prompted with the corresponding $icode container_id$$.
# The command
# $codei%
#	%driver% rm %container_id%
# %$$
# will remove the container.
# If the container is still running, you will need to use
# $codei%
#	%driver% rm --force %container_id%
# %$$
#
# $subhead Removing Images$$
# You can remove an old image using the command
# $codei%
#	%driver% rmi %name%
# %$$
# For example, $icode name$$ could be
# $code dismod_at.base$$,
# $code dismod_at.mixed$$, or
# $code dismod_at.image$$.
# You can keep the old image, under a different name, using the commands
# $codei%
#	%driver% tag %name% %different_name%
#	%driver% rmi %name%
# %$$
#
# $subhead Dockerfile$$
# The $code build$$ syntax will create the file
# $href%https://docs.docker.com/glossary/?term=Dockerfile%Dockerfile%$$
# in the current working directory.
# If such a file already exists, it will need to be moved or deleted.
#
# $subhead Errors$$
# $list number$$
# If you get the error message
# $codei%
#	 Unable to fetch some archives, maybe run apt-get update %...%
# %$$
# There may be an old OCI image result for $code apt-get update$$
# that is out of date.
# You can list the images using the command $code OCI images$$.
# Try removing an old image that corresponds to a previous
# $code apt-get update$$ and then re-run the
# $code dock_dismod_at.sh build$$ command.
#
# $lnext
# If you get the error message
# $codei%
#   Release file for %package% is not valid yet %...%
# %$$
# You system clock may be out of date (reporting an old day or time).
# Try fixing the system clock.
#
# $lend
#
# $head Run New Container$$
# Once $code dismod_at.image$$ has been created, you use the
# $icode build_type$$ syntax to run dismod_at in a container.
#
# $subhead Removing Containers$$
# The dismod_at container for a particular $icode user$$ will be named
# $codei%dismod_at.%user%$$.
# If such a container already exists,
# you will be prompted with the corresponding $icode container_id$$.
# The command
# $codei%
#	%driver% rm %container_id%
# %$$
# will remove the container.
# If the container is still running, you will need to use
# $codei%
#	%driver% rm --force %container_id%
# %$$
#
# $subhead build_type$$
# The $icode build_type$$ syntax will run the correspond
# $cref command$$ in the OCI image.
# The argument $icode build_type$$ must be either $code debug$$ or
# $code release$$.
# The $code release$$ version should be much faster.
# The $code debug$$ version will do more extensive error checking.
#
# $subhead database$$
# The second argument $icode database$$ must be a dismod_at database
# in the current working directory.
# This the first argument to the corresponding dismod_at command.
#
# $subhead command$$
# The third argument $icode command$$ must be one of the dismod_at commands.
# This is the second argument in the corresponding dismod_at command.
# The rest of the arguments $icode ...$$ are
# the same as the corresponding arguments for the $cref command$$.
#
# $subhead Other Arguments$$
# The other arguments to $code dock_dismod_at.sh$$ are the same as in the
# syntax for the $cref command$$,
# except that $code dismod_at$$ or $code dismodat.py$$
# have been replaced by $codei%dock_dismod_at.sh %build_type%$$.
#
# $head Debugging$$
# Some times an error occurs during the running of a container
# and you would like to go inside the container and execute commands.
# The following instructions are useful for this:
#
# $subhead Determine Container Id$$
# $codei%
#	%driver% ps -a
# %$$
# If $icode driver$$ is podman, the following might work better
# $codei%
#	podman ps --all --storage
# %$$
#
# $subhead Start Container$$
# If a container status is $code Exited$$, you can start it using:
# $codei%
#	%driver% start %container_id
# %$$
#
# $subhead Run Container$$
# If a container status is $code Up$$, you can run it using:
# $codei%
#	%driver% exec -it %container_id% bash
# %$$
# You will be in the container until you $code exit$$
# the $code bash$$ shell that is run by the command above.
#
# $subhead Stop Container$$
# If a container status is $code Up$$, you can stop it using:
# $codei%
#	%driver% stop %container_id
# %$$
#
# $end
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
grep "$at_cascade_version" doc.xsrst > /dev/null && \
sed -i bin/check_all.sh -e '/run_sphinx.sh/d'

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
