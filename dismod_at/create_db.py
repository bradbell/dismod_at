# $begin data_db$$ $newlinech #$$
# $spell
#	covariate
#	covariates
#	sqlalchemy
#	stdev
#	len
# $$
#
# $section Create An Empty Data Table$$
# 
# $head Syntax$$
# $codei%input_db(%connection%, %covariate_list%)
# %$$
#
# $head connection$$
# is an $code sqlalchemy$$ connection to the database.
#
# $head covariate_list$$
# is a list of strings where each string is the name 
# of a covariate that will correspond to a column in the data file.
# Each covariate name must begin with the characters $code c_$$.
#
# $head data$$
# The $code data$$ table is created in the database specified by 
# $icode connection$$.
#
# $subhead data_id$$
# The first column is an $code integer$$ and is the primary key for this table.
# It is intended that $code NULL$$ will be used for the value of this column
# during insertion. The actual value stored will auto-increment;
# i.e., be one larger with each insert. 
#
# $subhead integrand_id$$
# The next column is an $code integer$$ and identifies the integrand
# for this row by the primary key in the $code integrand$$ table.
#
# $subhead likelihood_id$$
# The next column is an $code integer$$ and identifies the data likelihood
# for this row by the primary key in the $code likelihood$$ table.
#
# $subhead location_id$$
# The next column is an $code integer$$ and identifies the location
# for this row by the primary key in the $code location$$ table.
#
# $subhead meas_value$$
# The next column is a $code real$$ and is the measured value
# for this row; i.e., the measurement of the integrand at the specified
# location etc. 
#
# $subhead meas_stdev$$
# The next column is a $code real$$ and is the standard deviation 
# corresponding to the measurement value.
# This standard for the data and is in the same units at the data.
#
# $subhead age_lower$$
# The next column is a $code real$$ and is the lower age limit 
# for this measurement and must be between the 
# minimum and maximum age specified by the $code age_grid$$ table.
#
# $subhead age_upper$$
# The next column is a $code real$$ and is the upper age limit
# for this measurement and must be between the 
# minimum and maximum age specified by the $code age_grid$$ table.
#
# $subhead time_lower$$
# The next column is a $code real$$ and is the lower time limit 
# for this measurement and must be between the 
# minimum and maximum time specified by the $code time_grid$$ table.
#
# $subhead time_upper$$
# The next column is a $code real$$ and is the upper time limit
# for this measurement and must be between the 
# minimum and maximum time specified by the $code time_grid$$ table.
#
# $subhead Covariates$$
# The next $codei%len(%covariate_list%)%$$ columns are $code real$$ 
# and are the values of the corresponding covariates for this row.
#
# $end
# ---------------------------------------------------------------------------
# 
import sqlalchemy as sql
def create_data(connection , covariate_list ) :
	# absolute path needs special case under windows; see
	# http://docs.sqlalchemy.org/en/rel_0_7/dialects/
	#	sqlite.html#connect-strings
	# server  = 'sqlite:///' + file_name
	# engine  = sql.create_engine( server )
	# connection = engine.connect()
	#
	cmd  = '''create table data(
		data_id        integer primary key,
		integrand_id   integer,
		data_like_id   integer,
		location_id    integer,
		meas_value     real.
		meas_stdev     real,
		age_lower      real,
		age_upper      real,
		time_lower     real,
		time_upper     real'''
	for covariate in covariate_list : 
		cmd += ',\n\t\t' + covariate + ' real'
	cmd += '\n\t)'
	connection.execute(cmd)
