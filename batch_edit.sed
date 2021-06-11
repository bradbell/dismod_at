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
#	omh/whats_new/2015.omh
# '
# list of files and or directories that are moved to new names
# move_paths='
#	devel/utility/data_subset.cpp
#	include/dismod_at/data_subset.hpp
# '
# list of sed commands that map old file and directory names to new names.
# The characters @s, @d, @n get converted to a space, dollar sign, new line.
# move_seds='
#	s|data_subset|subset_data|
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
s|DISMOD_AT_DATA_SUBSET_HPP|DISMOD_AT_SUBSET_DATA_HPP|
s|$begin data_subset\$|$begin subset_data$|g
s|$cref data_subset\$|$cref subset_data$|g
s|$cref/\([a-z_]*\)/data_subset/|$cref/\1/subset_data/|g
s|data_subset.cpp|subset_data.cpp|g
s|data_subset.hpp|subset_data.hpp|g
s|data_subset_struct|subset_data_struct|g
s|data_subset_obj|subset_data_obj|g
s|data_subset_cov_value|subset_data_cov_value|g
s|data_subset(|subset_data(|g
