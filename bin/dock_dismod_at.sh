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
# Under Construction
# ---------------------------------------------------------------------------
if [ "$2" == '' ] && [ "$1" != 'build' ]
then
	echo 'usage: dock_dismod_at.sh build'
	echo 'usage: dock_dismod_at.sh database command ...'
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

# Change the install prefix to /home/prefix and install
WORKDIR   /home/dismod_at.git
RUN mkdir /home/prefix && \
	sed -i bin/run_cmake.sh -e 's|\$HOME/|/home/|g' && \
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
database="$1"
if [ ! -f "$database" ]
then
	echo 'dock_dismod_at.sh Error'
	echo "cannot find the database $database"
	exit 1
fi
if [ -e work ]
then
	echo '/dock_dismod_at.sh Error'
	echo 'Must first remove the ./work directory'
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
# copy work.sh to the container
cat << EOF > work.sh
PATH="/home/prefix/dismod_at/bin:\$PATH"
#
export PYTHONPATH=\`find -L /home/prefix/dismod_at -name site-packages\`
dismodat.py example.db db2csv
exit 0
EOF
echo "docker cp work.sh $container_id:/home/work/work.sh"
docker cp 'work.sh' "$container_id:/home/work/work.sh"
#
# start up the container
echo "docker start $container_id"
docker start $container_id
#
# execute work.sh
echo "docker exec -it $container_id bash work.sh"
docker exec -it $container_id bash work.sh
#
# stop the container
echo "docker stop $container_id"
docker stop $container_id
#
# copy the result back
echo "docker cp $container_id:/home/work work"
docker cp $container_id:/home/work work
#
echo 'dismod_at.sh: OK'
exit 0
# ----------------------------------------------------------------------------
