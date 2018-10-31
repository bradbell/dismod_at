# Starting here: all lines must begin with a # character:
# ----------------------------------------------------------------------------
#
# list of directories that are added to the repository by batch_edit.sh
# new_directories='
# '
# list of files that are deleted by batch_edit.sh
# delete_files='
# '
# list of files that are not edited by the sed commands in this file
# (with the possible exception of the extra_sed commands)
# ignore_files='
# '
# list of files and or directories that are moved to new names
# move_paths='
# devel/utility/avg_age_grid.cpp
# example/devel/utility/avg_age_grid_xam.cpp
# example/user/avg_age_split.py
# include/dismod_at/avg_age_grid.hpp
# omh/table/avg_age_table.omh
# '
# sed command that maps ode file and or directory names to new file names
# move_sed='
#	s|avg_age_|age_avg_|
# '
# list of files that re edited by the extra_sed command
# extra_files='
# '
# sed command that is applied to the extra files
# (after the other sed commands in this file)
# extra_sed='
# '
# ----------------------------------------------------------------------------
# Starting here: lines with sed commands do not begin with a # character.
s|avg_age_|age_avg_|g
s|AVG_AGE_|AGE_AVG_|g
s|Average Age|Age Average|
s|average age|age average|
