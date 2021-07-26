"""
 --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
---------------------------------------------------------------------------
$begin sim_data$$ $newlinech #$$
$spell
	Dismod
	sim
	integrators
	std
	pini
	tol
$$

$section Simulate One Dismod_at Data Value$$

$head Under Construction$$
This routine is under construction because some if its test
are not yet passing.

$head Syntax$$
$codei%meas_value% = dismod_at.sim_data(
	%rate%, %integrand_name%, %grid%, %noise%, %abs_tol%
)%$$

$head Purpose$$
The $cref simulate_command$$ is a much faster way to simulate data.
This routine is easier to use and provides independent testing of the
dismod_at integrators.

$head rate$$
This is a dictionary and it's possible keys are the
$cref/rate names/rate_table/rate_name/$$.
If a $icode key$$ is a key in the dictionary,
$codei%
	%value% = %rate%[%key%](%age%, %time%)
%$$
returns a float equal to the value of the specified rate
at the specified age and time.
Note that $icode age$$ will always be zero when evaluating
$codei%
	%value% = %rate%['pini'](%age%, %time%)
%$$
If a rate name is not in the dictionary, the corresponding rate is zero.

$head integrand_name$$
This string is one of the
$cref/integrand names/integrand_table/integrand_name/$$.

$head grid$$
This argument defines the grid for trapezoidal integration.
(The dismod_at functions are often non-smooth and so a low order integration
method is called for.)
The $icode grid$$ is a dictionary with the following keys:

$subhead age$$
$icode%grid%['age']%$$ is a list of floats containg the grid points
for the average w.r.t. age.
These points are monotone increasing, the first (last) point is the
lower (upper) age limit for the average

$subhead time$$
$icode%grid%['time']%$$ is a list of floats containg the grid points
for the average w.r.t. time.
These points are monotone increasing, the first (last) point is the
lower (upper) time limit for the average

$head noise$$
This is a dictionary with the following possible keys:

$subhead density_name$$
$icode%noise%['density_name']%$$ is a string equal to one of the
$cref/density names/density_table/density_name/$$.

$subhead meas_std$$
$icode%noise%['meas_std']%$$ is a float equal to the
$cref/meas_value/data_table/meas_value/$$ for the simulated data.

$subhead eta$$
$icode%noise%['eta']%$$ is a float equal to
$cref/eta/data_table/eta/$$ for the simulated measurement.
This is only used when $icode density_name$$
is a $cref/log scaled density/density_table/Notation/Log Scaled/$$.

$subhead nu$$
$icode%noise%['nu']%$$ is a float equal to
$cref/nu/data_table/nu/$$ for the simulated measurement.
This is only used when $icode density_name$$
is $code students$$ or $code log_students$$.

$subhead Restriction$$
The $icode meas_std$$ must be zero.
This restriction will be removed in the future.

$head abs_tol$$
This float is an absolute error bound, that the integrator will achieve.

$head meas_value$$
This is the simulated
$cref/meas_value/data_table/meas_value/$$.

$children%example/user/sim_data.py
%$$
$head Example$$
The file $cref user_sim_data.py$$ contains an example and test of this routine.
$end
---------------------------------------------------------------------------
"""
# trapezoidal_average_1d
def trapezoidal_average_1d(fun, grid) :
	if len(grid) == 1 :
		return fun( grid[0] )
	#
	n_interval = len(grid) - 1
	integral     = 0.0
	fun_previous = fun( grid[0] )
	for j in range(n_interval) :
		step        = grid[j+1] - grid[j]
		fun_next    = fun(grid[j+1])
		integral    += (fun_previous + fun_next) * step / 2.0
		fun_previous = fun_next
	return integral / (grid[-1] - grid[0])
#
# trapezoidal_average_2d
def trapezoidal_average_2d(fun, age_grid, time_grid) :
	assert len(age_grid) > 0
	assert len(time_grid) > 0
	#
	# fun_time = average w.r.t age
	def fun_time(t) :
		# fun_age
		def fun_age(a) :
			return fun(a, t)
		return trapezoidal_average_1d(fun_age, age_grid)
	#
	return trapezoidal_average_1d(fun_time, time_grid)
#
# SC_fun
def SC_fun(a, t, rate, abs_tol) :
	import scipy.integrate
	#
	# Overloading the functions S and C, we define the ODE
	# S'(s) = - [ iota(a+s,t+s) + omega(a+s,t+s) ] * S(s)
	#         + rho(a+s,t+s) * C(s)
	# C'(s) = - [ rho(a+s,t+s) + omega(a+s,t+s) + chi(a+s,t+s) ] * C(s)
	#         + i(a+s,t+s) * S(s)
	# with initial values
	# S(-a)  = pini(0, t-a)
	# C(-a)  = 1 - S(-a)
	#
	# Then we have that
	# S(a,t) = S(0)
	# C(a,t) = C(0)
	#
	# f
	def f(s, SC) :
		iota  =  rate['iota'](a+s, t+s)
		rho   =   rate['rho'](a+s, t+s)
		chi   =   rate['chi'](a+s, t+s)
		omega = rate['omega'](a+s, t+s)
		#
		Sp = - (iota + omega) * SC[0] +                  rho * SC[1]
		Cp =             iota * SC[0] - (rho + omega + chi ) * SC[1]
		return [ Sp, Cp ]
	#
	# jac
	def jac(s, SC) :
		iota  =  rate['iota'](a+s, t+s)
		rho   =   rate['rho'](a+s, t+s)
		chi   =   rate['chi'](a+s, t+s)
		omega = rate['omega'](a+s, t+s)
		#
		return [ [ -(iota + omega) , rho ], [ iota, -(rho + omega + chi) ] ]
	#
	# S(-a), C(-a)
	Cini = rate['pini'](0, t - a)
	Sini = 1.0 - Cini
	#
	# SC_ode
	SC_ode = scipy.integrate.ode(f, jac)
	SC_ode.set_integrator('vode', atol = abs_tol)
	SC_ode.set_initial_value( [Sini, Cini], - a )
	#
	# integrate ODE from s = -a to s = 0
	SC = SC_ode.integrate(0.0)
	return SC
#
# integrand_fun
def integrand_fun(a, t, rate, integrand_name, abs_tol) :
	import scipy.integrate
	#
	# iota, rho, chi, omega
	iota  =  rate['iota'](a, t)
	rho   =   rate['rho'](a, t)
	chi   =   rate['chi'](a, t)
	omega = rate['omega'](a, t)
	#
	# no ODE cases
	integrand_value = {
		'Sincidence' :  iota,
		'remission'  :  rho,
		'mtexcess'   :  chi,
		'mtother'    :  omega,
		'mtwith'     :  omega + chi,
	}
	if integrand_name in integrand_value :
		return integrand_value[integrand_name]
	#
	# avoid division by zero (when omeage zero) in cases above
	if integrand_name == 'relrisk' :
		return (omega + chi) / omega
	#
	# ODE cases
	(S, C) = SC_fun(a, t, rate, abs_tol)
	P      = C / (S + C)
	integrand_value = {
		'susceptible' : S,
		'withC'       : C,
		'prevalence'  : P,
		'Tincidence'  : iota * (1 - P),
		'mtspecific'  : chi * P,
		'mtall'       : omega + chi * P,
	}
	if integrand_name in integrand_value :
		return integrand_value[integrand_name]
	#
	# avoid division by zero (when omeage and P ae zero) in cases above
	if integrand_name == 'mtstandard' :
		return (omega + chi) / (omega + chi * P)
	#
	print('sim_data: ' + integrand_name + ' is not a valid integrand name')
	assert False
#
# sim_data
def sim_data(rate, integrand_name, grid, noise, abs_tol) :
	import scipy.integrate
	assert noise['meas_std'] == 0.0
	#
	# zero_fun
	def zero_fun(a, t) :
		return 0.0
	#
	# rate_extended
	rate_extended = dict()
	for key in ['pini', 'iota', 'rho', 'chi', 'omega'] :
		if key in rate :
			rate_extended[key] = rate[key]
		else :
			rate_extended[key] = zero_fun
	#
	# grids
	age_grid  = grid['age']
	time_grid = grid['time']
	#
	# function we will average
	def func(a, t) :
		return integrand_fun(
			a, t, rate_extended, integrand_name, abs_tol
		)
	#
	avg = trapezoidal_average_2d(func, age_grid, time_grid)
	#
	return avg
