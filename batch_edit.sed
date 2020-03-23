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
#	omh/table/hessian_table.omh
#	example/user/hessian_math.py
# '
# list of sed commands that map old file and directory names to new names.
# The characters @s, @d, @n get converted to a space, dollar sign, new line.
# move_seds='
#	s|omh/table/hessian_table.omh|omh/table/hes_fixed_table.omh|
#	s|example/user/hessian_math.py|example/user/hes_fixed_math.py|
# '
# list of files that get edited by the extra_seds command
# extra_files='
#	omh/table/hes_fixed_table.omh
#	devel/cmd/sample_command.cpp
# '
# list of sed commands that are applied to the extra files,
# after the other sed commands in this file.
# The characters @s, @d, @n get converted to a space, dollar sign, new line.
# extra_seds='
#	s|@dspell|&@n\thes|
# '
# ----------------------------------------------------------------------------
# Put other sed commands below here and without # at start of line
s|omh/table/hessian_table.omh|omh/table/hes_fixed_table.omh|
s|hessian_id|hes_fixed_id|g
s|hessian_value|hes_fixed_value|g
s|hessian_table|hes_fixed_table|g
s|'hessian'|'hes_fixed'|g
s|"hessian"|"hes_fixed"|g
#
s|hessian_math|hes_fixed_math|g
