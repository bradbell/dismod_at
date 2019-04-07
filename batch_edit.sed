# Plan for future changes on master branch
# 1. s|^\t\(  *\)GNU Affero|        GNU Affero|
# 2. aplahbetical sort of headings in option table
# ----------------------------------------------------------------------------
# None of the lists below can have white space in an entry.
#
# list of directories that are added to the repository by batch_edit.sh
# new_directories='
# '
# list of files that are deleted by batch_edit.sh
# delete_files='
# '
# list of files that are not edited by the sed commands in this file
# (with the possible exception of the extra_seds commands)
# ignore_files='
# '
# list of files and or directories that are moved to new names
# move_paths='
#	devel/model/avg_std_effect.cpp
#	include/dismod_at/avg_std_effect.hpp
#	example/devel/model/avg_std_effect_xam.cpp
# '
# list of sed commands that maps old file and or directory names to new
# file names (the characters @s gets converted to a single space)
# move_seds='
#	s|avg_std|avg_noise|
# '
# list of files that get edited by the extra_seds command
# extra_files='
# '
# list of sed commands that are applied to the extra files,
# after the other sed commands in this file
# (the characters @s gets converted to a single space)
# extra_seds='
# '
# ----------------------------------------------------------------------------
# Put other sed commands below here and without # at start of line
s|_AVG_STD_|_AVG_NOISE_|g
s|standard deviation effect|noise effect|g
s|avg_std_effect  |avg_noise_effect|g
#
s|avg_std|avg_noise|g
s|avgstd|avg_noise|g

