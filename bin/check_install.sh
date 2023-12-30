#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
# test install of python module and executable
# ---------------------------------------------------------------------------
if [ "$0" != "bin/check_install.sh" ]
then
   echo "bin/check_install.sh: must be executed from its parent directory"
   exit 1
fi
if [ "$1" != 'debug' ] && [ "$1" != 'release' ]
then
   echo 'bin/check_all.sh build_type'
   echo 'build_type is not debug or release'
   exit 1
fi
if [ "$1" == debug ]
then
   bin/run_cmake.sh --debug
else
   bin/run_cmake.sh
fi
# -----------------------------------------------------------------------------
# bash function that echos and executes a command
echo_eval() {
   echo $*
   eval $*
}
# ---------------------------------------------------------------------------
# dismod_at_prefix
cmd=`grep ^dismod_at_prefix bin/run_cmake.sh`
eval $cmd
#
# python3_executable
cmd=`grep ^python3_executable bin/run_cmake.sh`
eval $cmd
#
# get python site-packages directory
python_dir=''
if find -L $dismod_at_prefix -name site-packages > /dev/null
then
   python_dir=`find -L $dismod_at_prefix -name site-packages`
fi
bin_dir="$dismod_at_prefix/bin"
#
# remove old install executable and python packages
echo_eval rm -rf $bin_dir/dismod_at
echo_eval rm -rf $bin_dir/dismodat.py
if [ "$python_dir" != '' ]
then
   echo_eval rm -rf $pyton_dir/dismod_at
fi
#
# install fresh copy
echo_eval cd build
echo_eval make install install_python
echo_eval cd ..
#
# PATH
export PATH="$bin_dir:$PATH"
#
# PYTHONPATH
python_dir=`find -L $HOME/prefix/dismod_at -name site-packages`
export PYTHONPATH="$python_dir"
echo "PYTHONPATH=$PYTHONPATH"
#
# LD_LIBRARY_PATH
export LD_LIBRARY_PATH=''
#
# create check_install and change into it
if [ -e 'build/check_install' ]
then
   echo_eval rm -r build/check_install
fi
echo_eval mkdir build/check_install
echo_eval cd build/check_install
echo_eval cp ../../example/get_started/get_started_db.py get_started_db.py
#
# create get_started.db
cat << EOF > create_db.py
import get_started_db
get_started_db.get_started_db()
EOF
cat << EOF > run.sh
$python3_executable create_db.py
dismod_at get_started.db init
dismodat.py get_started.db db2csv
EOF
chmod +x run.sh
cat run.sh
./run.sh
#
dismod_at_image='dismod_at.image.release'
if ! podman images | grep "^localhost/$dismod_at_image" > /dev/null
then
   dismod_at_image='dismod_at.image.debug'
   if ! podman images | grep "^localhost/$dismod_at_image" > /dev/null
   then
      echo 'podman cannot find dismod_at.image, skipping its test'
      echo 'check_install.sh: OK'
      exit 0
   fi
fi
# ---------------------------------------------------------------------------
# Test that OCI image gives the same result
#
# prefix
cmd=`grep ^prefix ../../bin/dock_dismod_at.sh`
eval $cmd
if [ "$prefix" != '/home/venv' ]
then
   echo "dock_dismod_at.sh: prefix = $prefix"
   exit 1
fi
cat << EOF > run.sh
source $prefix/bin/activate
python3 create_db.py
dismod_at get_started.db init
dismodat.py get_started.db db2csv
EOF
echo_eval mkdir podman
echo_eval cd podman
cat ../run.sh
if podman ps -a | grep test_container > /dev/null
then
   podman rm -f test_container
fi
echo 'exit 0' | podman run -i --name test_container $dismod_at_image
list='get_started_db.py create_db.py run.sh'
for file in $list
do
   podman cp ../$file test_container:/home/work
done
podman start test_container
echo './run.sh ; exit 0' | podman exec -i test_container bash
list='age_avg.csv  data.csv  option.csv  variable.csv'
for file in $list
do
   podman cp test_container:/home/work/$file $file
   if ! diff ../$file $file
   then
      echo "docker image results for $file are different"
      exit 1
   fi
done
# -----------------------------------------------------------------------------
echo 'check_install.sh: OK'
exit 0
