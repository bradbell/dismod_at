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
# '
# list of sed commands that are applied to the extra files,
# after the other sed commands in this file.
# The characters @s, @d, @n get converted to a space, dollar sign, new line.
# extra_seds='
# '
# ----------------------------------------------------------------------------
# Put other sed commands below here and without # at start of line
s|cref/no sample table/sample_command/sample_table/No Sample Table/|cref/no sample table/sample_command/Output Tables/No Sample Table/|
s|cref/fit_var_table/sample_command/fit_var_table/|cref/fit_var_table/sample_command/Extra Input Tables/fit_var_table/|
s|cref/hes_fixed table/sample_command/hes_fixed_table/|cref/hes_fixed table/sample_command/Output Tables/hes_fixed_table/|
s|/sample_command/method/asymptotic/|/sample_command/asymptotic/|
s|/sample_command/method/simulate/|/sample_command/simulate/|
