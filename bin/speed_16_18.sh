#! /bin/bash -e
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# compare the speed of last version in 2016 with last version in 2018
# ----------------------------------------------------------------------------
remote='https://github.com/bradbell/dismod_at.git'
random_seed='12345' # you can change this to test a different data set
quasi_fixed='false' # you can change this: 'false'=Newton, 'true'=quasi-Newton
# ----------------------------------------------------------------------------
if [ "$0" != 'bin/speed_16_18.sh' ]
then
   echo 'bin/speed_16_18.sh: must be executed from its parent directory'
   exit 1
fi
# ----------------------------------------------------------------------------
checkout_version() {
   hash_code="$1"
   version="$2"
   dir="$HOME/trash/dismod_at-$version"
   if [ ! -e $dir ]
   then
      echo_eval git clone $remote $dir
   fi
   cd $dir
   git reset --hard
   git checkout --quiet $hash_code
   check=`version.sh get`
   if [ "$version" != "$check" ]
   then
      echo "hash    = $hash_code"
      echo "version = $version"
      echo "check   = $check"
      exit 1
   else
      echo "created $dir"
   fi
}
install_package() {
   package="$1"
   dir="$2"
   if [ ! -e "$dir" ]
   then
      bin/install_$package.sh
   else
      pushd $dir/build
      make install
      popd
   fi
}
#
hash_2016='df4ae4b9178f3e851a30096f6687430cbf3b34ed'
hash_2018='2fb1dd7b0541090190382e01177f289b75e67004'
#
version_2016='20161203'
version_2018='20181219'
#
checkout_version $hash_2016 $version_2016
checkout_version $hash_2018 $version_2018
#
# copy example/user/speed.py from version_2018 to version_2016
old_file="$HOME/trash/dismod_at-$version_2016/speed/simulated.py"
new_file="$HOME/trash/dismod_at-$version_2018/example/user/simulated.py"
#
sed -i $old_file \
   -e "/quasi_fixed/s|'value':'true'|'value':'$quasi_fixed'|" \
   -e "/quasi_fixed/s|'value':'false'|'value':'$quasi_fixed'|"
#
cp $old_file $new_file
cat << EOF > junk.sed
s|speed/simulated.py|example/user/simulated.py|
#
/^\\t*avgint_table\$/d
s|^\\t*covariate_table,\$|&\\
      avgint_table,|
s|^\\t*smooth_table,\$|&\\
      dict(),|
#
s|'start', 'fit'|'fit'|
s|simulate_index = '0'|&\\
      cmd.append('both')|
s|'variable_value'|'fit_var_value'|
EOF
sed -i $new_file -f junk.sed
# ----------------------------------------------------------------------------
# version_2016
# ----------------------------------------------------------------------------
cd "$HOME/trash/dismod_at-$version_2016"
list='
   install_suitesparse.sh
   install_eigen.sh
   install_ipopt.sh
   install_cppad.sh
   install_cppad_mixed.sh
   run_cmake.sh
'
for file in $list
do
   sed -i bin/$file \
      -e "s|^build_type=.*|build_type='release'|" \
      -e "s|^log_fatal_error=.*|log_fatal_error='YES'|"
   #
   if ! grep "^build_type='release'" bin/$file
   then
      echo "expected build_type to be release in bin/$file"
      exit 1
   fi
done
#
dir='build/external/SuiteSparse'
if [ ! -e "$dir" ]
then
   bin/install_suitesparse.sh
else
   pushd $dir
   make install
   popd
fi
#
dir='build/external/eigen-3.2.9'
install_package eigen $dir
#
dir='build/external/Ipopt-3.12.6'
install_package ipopt $dir
#
dir='build/external/cppad-20161012'
install_package cppad $dir
#
dir='build/external/cppad-20161012'
install_package cppad $dir
#
dir='build/external/cppad_mixed-20161016'
if [ ! -e $dir ]
then
   if ! bin/install_cppad_mixed.sh
   then
      echo 'speed_16_18.sh: expected this install to fail'
      echo 'will fix and re-install'
   fi
fi
pushd $dir
file='include/cppad/mixed/cppad_mixed.hpp'
list='
   ran_obj_tst
'
for name in $list
do
   if ! grep "extern bool $name" $file > /dev/null
   then
      sed -i $file \
         -e "s|// private examples|&\nextern bool $name(void);|"
   fi
done
cd build
make install
popd
# ----------------------------------------------------------------------------
# need sandbox at front of system path
sed -i speed/simulated.py \
   -e 's|sys.path.append(|sys.path.insert(0, |'
#
bin/run_cmake.sh
cd build
make speed
cd ..
#
echo "python3 speed/simulated.py $random_seed > simulated.out"
python3 speed/simulated.py $random_seed > simulated.out
#
echo_eval cd build/speed
echo_eval massif.sh ../devel/dismod_at example.db fit 0
echo_eval mv massif.out ../../massif.out
# ----------------------------------------------------------------------------
# version_2018
# ----------------------------------------------------------------------------
cd "$HOME/trash/dismod_at-$version_2018"
if ! grep "^build_type='release'" bin/run_cmake.sh
then
   echo 'expected build_type to be release in bin/run_cmake.sh'
   exit 1
fi
#
dir='build/external/eigen-3.2.9'
install_package eigen $dir
#
dir='build/external/Ipopt-3.12.6'
install_package ipopt $dir
#
dir='build/external/cppad-20180925'
install_package cppad $dir
#
dir='build/external/cppad_mixed-20180925'
install_package cppad_mixed $dir
# ----------------------------------------------------------------------------
bin/run_cmake.sh
cd build
make speed
cd ..
#
echo "python3 example/user/simulated.py $random_seed > simulated.out"
python3 example/user/simulated.py $random_seed > simulated.out
#
echo_eval cd build/example/user
echo_eval massif.sh ../../devel/dismod_at example.db fit both 0
echo_eval mv massif.out ../../../massif.out
# ----------------------------------------------------------------------------
cat << EOF
Compare result speed in following files:
   $HOME/trash/dismod_at-$version_2016/simulated.out
   $HOME/trash/dismod_at-$version_2018/simulated.out
Compare memory results in following files:
   $HOME/trash/dismod_at-$version_2016/massif.out
   $HOME/trash/dismod_at-$version_2018/massif.out
EOF
# ----------------------------------------------------------------------------
echo 'junk.sh: OK'
exit 0
