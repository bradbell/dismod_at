#! /usr/bin/env bash
# Fix old versions of dock_dismod_at.sh so they work with the current
# cppad_mixed repository which renamed its master branch as main.
set -e -u
#
if [ ! -e dock_dismod_at.sh ]
then
   echo 'patch_dock_dismod_at.sh: dock_dismod_at.sh is not in current directory'
   eixt 1
fi
if grep 'git checkout main' dock_dismod_at.sh > /dev/null
then
   echo 'patch_dock_dismod_at.sh: looks like this patch was already applied'
   exit 1
fi
#
# sed.$$
cat << EOF > sed.$$
/^RUN bin[/]get_cppad_mixed.sh/! b end
s|^|sed -i bin/get_cppad_mixed.sh -e 's/git checkout master/git checkout main/'\\n|
#
: end
EOF
#
# dock_dismod_at.sh
sed -i dock_dismod_at.sh -f sed.$$
#
rm sed.$$
echo 'patch_dock_dismod_at.sh: OK'
exit 0


