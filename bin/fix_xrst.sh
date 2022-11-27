#! /bin/bash -e
# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# -----------------------------------------------------------------------------
edit_file()
{  file="$1"
   echo $file
   cp $file ~/trash
   sed -i $file -f temp.sed
   echo '--------------------------------------------------------------------'
   if diff $file ~/trash
   then
      echo "$file: no changes"
   else
      echo "$file: changes above"
   fi
}
# ----------------------------------------------------------------------------
cat << EOF > temp.sed
/:math:\`\\\\newcommand/d
EOF
edit_file dev.xrst
# ----------------------------------------------------------------------------
cat << EOF > temp.sed
/:math:\`\\\\newcommand/d
#
s|{xrst_begin dismod_at}|&\\
{xrst_spell\\
   github\\
}|
#
: end
EOF
edit_file doc.xrst
# -----------------------------------------------------------------------------
cat << EOF > temp.sed
/build developer documentation/! b end
N
N
N
N
s/.*/# build developer documentation\\
if [ -e doc ]\n\
then\n\
   echo_eval rm -r doc\n\
fi\n\
echo_eval xrst \\\\\\
   --local_toc \\\\\\
   --html_theme sphinx_rtd_theme \\\\\\
   --output_dir doc/
#
: end
EOF
edit_file bin/check_all.sh
# ----------------------------------------------------------------------------
cat << EOF > temp.sed
s|doc.omh|doc.xrst|g
s|dev.omh|dev.xrst|g
EOF
edit_file bin/devel.sh
# ----------------------------------------------------------------------------
cat << EOF > temp.sed
s|"..rref user_\$name"|":ref:.user_\$name.py-title"|
s|.[\$]rref user_\$name.py.[\$]|:ref:.user_\$name.py-title.|
EOF
edit_file bin/check_user_xam.sh
# ----------------------------------------------------------------------------
cat << EOF > temp.sed
s|omh|xrst|g
EOF
edit_file bin/check_user_xam.sh
# -----------------------------------------------------------------------------
echo 'fix_xrst.sh: OK'
exit 0
