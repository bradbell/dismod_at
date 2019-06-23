#! /bin/bash -e
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
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
# $$
#
# $section Install and Run dismod_at in a Docker Image$$
#
# $head Syntax$$
# $codei%dock_dismod_at.sh build
# %$$
# $codei%dock_dismod_at.sh %build_type% %database% %command% %...%
# %$$
#
# $head Purpose$$
# Using this script to build and run a docker image is an alternative to
# going through the steps required to $cref install_unix$$ dismod_at.
# You can get a copy of this script at the following link
# $href%https://raw.githubusercontent.com/bradbell/dismod_at/master/bin/dock_dismod_at.sh
#	%dock_dismod_at.sh
# %$$
# If understand docker, this script also serves as an example
# install of dismod_at.
#
# $head Requirements$$
# You must have a copy of $href%https://docs.docker.com/%docker%$$
# installed on your system.
# In addition, you must have permission to use it.
# You can test this on your system by trying to execute the following command:
# $codei%
#	docker run busybox echo 'Hello World'
# %$$
#
# $head Building Image$$
#
# $head dismod_at.image$$
# The $code build$$ syntax will create a new docker image with the name
# $code dismod_at.image$$.
# This command will not execute if such an image already exists.
# You can remove and old image using the command
# $codei%
#	docker rmi dismod_at.image
# %$$
# You can keep the old image, under a different name, using the commands
# $codei%
#	docker tag dismod_at.image %different_name%
#	docker rmi dismod_at.image
# %$$
#
# $subhead Dockerfile$$
# The $code build$$ syntax will create the file
# $href%https://docs.docker.com/glossary/?term=Dockerfile%Dockerfile%$$
# in the current working directory.
# If such a file already exists, it will need to be moved or deleted.
#
# $head Run Container$$
#
# $head build_type$$
# The $icode build_type$$ syntax will run the correspond
# $cref command$$ in the docker image.
# The argument $icode build_type$$ must be either $code debug$$ or
# $code release$$.
#
# $head Other Arguments$$
# The other arguments to $code dock_dismod_at.sh$$ are the same as in the
# syntax for the $cref command$$,
# except that $code dismod_at$$ or $code dismodat.py$$
# have been replaced by $codei%dock_dismod_at.sh% %build_type%$$.
#
# $end
# ---------------------------------------------------------------------------
if [ "$2" == '' ] && [ "$1" != 'build' ]
then
	echo 'usage: dock_dismod_at.sh build'
	echo 'usage: dock_dismod_at.sh build_type database command ...'
	exit 1
fi
# ---------------------------------------------------------------------------
# Build Docker image
# ----------------------------------------------------------------------------
if [ "$1" == 'build' ] && [ "$2" == '' ]
then
	if [ -e 'Dockerfile' ]
	then
		echo 'dock_dismod_at.sh Error'
		echo "Must first remove ./Dockerfile"
		exit 1
	fi
	if docker images | grep '^dismod_at.image ' > /dev/null
	then
		echo 'dock_dismod_at.sh Error'
		echo 'Must first remove following docker images:'
		docker images | head -1
		docker images | grep '^dismod_at.image '
		exit 1
	fi
	echo 'Creating Dockerfile'
cat << EOF > Dockerfile

# Ubuntu 19.04 with dismod_at requirements
FROM ubuntu:18.04
RUN  apt-get update
RUN  DEBIAN_FRONTEND=noninteractive apt-get install -y  \
cmake \
git \
gfortran \
libblas-dev \
liblapack-dev \
libsuitesparse-dev \
libgsl-dev \
pkg-config \
python3 \
python3-distutils-extra \
python3-matplotlib \
python3-numpy \
python3-scipy \
libsqlite3-dev \
vim \
wget

# Get current verison of dismod_at source
WORKDIR /home
RUN git clone https://github.com/bradbell/dismod_at.git dismod_at.git

# Change the install prefix to /home/prefix
WORKDIR   /home/dismod_at.git
RUN mkdir /home/prefix && sed -i bin/run_cmake.sh -e 's|\$HOME/|/home/|g'

# install debug version
RUN sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='debug'|" && \
	bin/example_install.sh

# install release version
RUN sed -i bin/run_cmake.sh -e "s|^build_type=.*|build_type='release'|" && \
	bin/example_install.sh

# start in /home/work directory
WORKDIR /home/work
EOF
#
	echo 'Creating dismod_at.image'
	docker build --tag dismod_at.image .
	#
	echo 'dock_dismod_at.sh build: OK'
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
if docker ps -a | grep " $container_name\$" > /dev/null
then
	echo 'dock_dismod_at.sh Error'
	echo 'Must first remove following docker containers:'
	docker ps -a | head -1
	docker ps -a | grep " $container_name\$"
	echo 'Use the following command for each container_id above:'
	echo 'docker rm contain_id'
	exit 1
fi
#
# create a new dismod_at container
echo "echo 'exit 0' | docker run -i --name $container_name dismod_at.image bash"
echo 'exit 0' | docker run -i --name "$container_name" dismod_at.image bash
#
# get the id for the new container
container_id=`docker ps -a -q --filter "name=$container_name"`
#
# copy the database to the container
echo "docker cp $database $container_id:/home/work/$database"
docker cp "$database"  "$container_id:/home/work/$database"
#
# A temporary directory
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
# Add to load library path
export LD_LIBRARY_PATH=\`find -L /home/prefix/dismod_at -name 'libipopt.*' | \
	head -1 | sed -e 's|/[^/]*\$||'\`
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
echo "docker cp $temporary_dir/work.sh $container_id:/home/work/work.sh"
docker cp "$temporary_dir/work.sh" "$container_id:/home/work/work.sh"
#
# start up the container
echo "docker start $container_id"
docker start $container_id
#
# execute work.sh
echo "docker exec -it $container_id ./work.sh"
docker exec -it $container_id ./work.sh
#
# copy the result back
echo "docker cp $container_id:/home/work $temporary_dir/work"
docker cp $container_id:/home/work "$temporary_dir/work"
#
# remove the container
echo "docker rm --force $container_id"
docker rm --force $container_id
#
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
echo "rm -r $temporary_dir"
rm -r $temporary_dir
#
echo 'dock_dismod_at.sh: OK'
exit 0
# ----------------------------------------------------------------------------
