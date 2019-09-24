# Plan for future changes on master branch
# 1. s|^\t\(  *\)GNU Affero|        GNU Affero|
# ----------------------------------------------------------------------------
# None of the lists below can have white space or a dollar sign in an entry.
#
# list of directories that are added to the repository by batch_edit.sh
# new_directories='
# '
# list of files that are deleted by batch_edit.sh
# delete_files='
# '
# List of files that are not edited by the sed commands in this file
# (with the possible exception of the extra_seds commands).
# The files in bin/devel.sh ignore_files are automatically in this list.
# ignore_files='
# '
# list of files and or directories that are moved to new names
# move_paths='
# '
# list of sed commands that map old file and directory names to new names.
# The characters @s, @d, @n get converted to a space, dollar sign, new line.
# move_seds='
# '
# list of files that get edited by the extra_seds command
# extra_files='
#   omh/install/unix.omh
# '
# list of sed commands that are applied to the extra files,
# after the other sed commands in this file.
# The characters @s, @d, @n get converted to a space, dollar sign, new line.
# extra_seds='
#   s|@dsubhead@seigen\@d|@dsubhead@sinstall_eigen.sh@d|
#   s|@dsubhead@sipopt\@d|@dsubhead@sinstall_ipopt.sh@d|
#   s|@dsubhead@sCppAD\@d|@dsubhead@sinstall_cppad.sh@d|
#   s|@dsubhead@scppad_mixed\@d|@dsubhead@sinstall_cppad_mixed.sh@d|
# '
# ----------------------------------------------------------------------------
# Put other sed commands below here and without # at start of line
s|/Special Requirements/eigen/|/Special Requirements/install_eigen.sh/|
s|/Special Requirements/ipopt/|/Special Requirements/install_ipopt.sh/|
s|/Special Requirements/CppAD/|/Special Requirements/install_cppad.sh/|
s|/Special Requirements/cppad_mixed/|/Special Requirements/install_cppad_mixed.sh/|
#
s%|bin/install_eigen.sh|install_unix|Special Requirements|eigen|%/install_eigen.sh/install_unix/Special Requirements/install_eigen.sh/%
