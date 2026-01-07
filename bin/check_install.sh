#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
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
build_type="$1"
# -----------------------------------------------------------------------------
# echo_eval
echo_eval() {
   echo $*
   eval $*
}
#
# dismod_at_prefix
eval $(bin/install_settings.py | grep '^dismod_at_prefix')
#
# python3_executable
eval $(bin/install_settings.py | grep '^python3_executable')
#
# site_packages
site_packages=''
if find -L $dismod_at_prefix -name site-packages > /dev/null
then
   site_packages=`find -L $dismod_at_prefix -name site-packages`
fi
if [ "$site_packages" != '' ]
then
   echo_eval rm -rf $python_dir/dismod_at
fi
#
# bin_dir
bin_dir="$dismod_at_prefix/bin"
echo_eval rm -rf $bin_dir/dismod_at
echo_eval rm -rf $bin_dir/dismod-at
#
# make install
if [ "$build_type" == debug ]
then
   bin/run_cmake.sh --debug
else
   bin/run_cmake.sh
fi
echo_eval cd build
echo_eval make install install_python
echo_eval cd ..
#
# PATH
export PATH="$bin_dir:$PATH"
#
# PYTHONPATH
site_packages=$(
   find -L $HOME/prefix/dismod_at -name site-packages  | tr '\n' ':'
)
export PYTHONPATH="$site_packages"
echo "PYTHONPATH=$PYTHONPATH"
#
# LD_LIBRARY_PATH
export LD_LIBRARY_PATH=''
#
# check_install
if [ -e 'build/check_install' ]
then
   echo_eval rm -r build/check_install
fi
echo_eval mkdir build/check_install
echo_eval cd build/check_install
#
# get_started_db.py
echo_eval cp ../../example/get_started/get_started_db.py get_started_db.py
#
# create_db.py
cat << EOF > create_db.py
import get_started_db
get_started_db.get_started_db()
EOF
#
# run.sh
cat << EOF > run.sh
$python3_executable create_db.py
dismod_at get_started.db init
dismod-at get_started.db db2csv
EOF
chmod +x run.sh
cat run.sh
./run.sh
# ---------------------------------------------------------------------------
# dismod_at_image
dismod_at_image='dismod_at.dismod_at.release'
if ! which podman > /dev/null
then
   echo "Cannot find podman, skipping test of $dismod_at_image"
   echo 'check_install.sh: OK'
   exit 0
else
   if ! podman images | grep "^localhost/$dismod_at_image" > /dev/null
   then
      dismod_at_image='dismod_at.dismod_at.debug'
      if ! podman images | grep "^localhost/$dismod_at_image" > /dev/null
      then
         echo "podman cannot find $dismod_at_image, skipping its test"
         echo 'check_install.sh: OK'
         exit 0
      fi
   fi
fi
#
# prefix
cmd=`grep ^prefix ../../bin/dock_dismod_at.sh`
eval $cmd
if [ "$prefix" != '/home/venv' ]
then
   echo "dock_dismod_at.sh: prefix = $prefix"
   exit 1
fi
#
# run.sh
cat << EOF > run.sh
python3 create_db.py
dismod_at get_started.db init
dismod-at get_started.db db2csv
EOF
#
# podman directory
echo_eval mkdir podman
echo_eval cd podman
#
# test_container
if podman ps -a | grep test_container > /dev/null
then
   podman rm -f test_container
fi
echo 'exit 0' | podman run -i --name test_container $dismod_at_image
list='
   get_started_db.py
   create_db.py
   run.sh
'
for file in $list
do
   podman cp ../$file test_container:/home/work
done
#
# test_container
cat ../run.sh
podman start test_container
echo './run.sh ; exit 0' | podman exec -i test_container bash
#
# compare podman results with local install results
list='
   age_avg.csv
   data.csv
   option.csv
   variable.csv
'
for file in $list
do
   podman cp test_container:/home/work/$file $file
   if ! diff ../$file $file
   then
      echo "podman image results for $file are different for local install"
      exit 1
   fi
done
# -----------------------------------------------------------------------------
echo 'check_install.sh: OK'
exit 0
