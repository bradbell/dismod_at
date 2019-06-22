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
# ----------------------------------------------------------------------------
if [ "$1" == 'build' ]
then
	if [ -d 'build' ]
		dockerfile_path='build'
	else
		dockerfile_path='.'
	fi
	if [ -e "$dockerfile_path/Dockfile" ]
	then
		echo 'dock_dismod_at.sh Error'
		echo "Must first remove $dockerfile_path/Dockfile"
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
cat << EOF > $dockerfile_path/Dockerfile
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

WORKDIR /home
RUN git clone https://github.com/bradbell/dismod_at.git dismod_at.git

RUN mkdir /home/prefix
WORKDIR   /home/dismod_at.git
RUN \
	sed -i bin/run_cmake.sh -e 's|\$HOME/|/home/|g' && \
	echo 'PATH="/home/prefix/dismod_at/bin:\$PATH"' >> /root/.bashrc && \
	bin/example_install.sh
COPY      * /home/work
ENV       PATH="/home/prefix/dismod_at/bin:\$PATH"
EOF
#
	echo 'Creating dismod_at.image'
	docker build --tag dismod_at.image --file Dockerfile $dockerfile_path
	#
	echo 'dock_dismod_at.sh build: OK'
	exit 0
fi
#
echo 'Use following to create and run the container:'
echo 'docker run -it --name dismod_at.container dismod_at.image bash'
#
echo 'dismod_at.sh: OK'
exit 0
# ----------------------------------------------------------------------------
