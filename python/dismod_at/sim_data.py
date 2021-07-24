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
$$

$section Simulate One Dismod_at Data Value$$

$head Syntax$$
$codei%meas_value% = dismod_at.sim_data(
	%rate%, %integrand_name%, %bound%, %noise%
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
If a rate name is not in the dictionary, the corresponding rate is zero.

$head integrand_name$$
This string is one of the
$cref/integrand names/integrand_table/integrand_name/$$.

$head bound$$
This dictionary has the following keys:
$code age_lower$$,
$code age_upper$$,
$code time_lower$$,
$code time_upper$$.
The dictionary values are floats equal to the corresponding limits for the
simulated data.

$subhead Restriction$$
The lower and upper limits must be equal.
This restriction will be removed in the future.

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

$subhead meas_value$$
This is the simulated
$cref/meas_value/data_table/meas_value/$$.

$end
---------------------------------------------------------------------------
"""
# SC_fun
def SC_fun(a, t, rate) :
	import scipy
	#
	# Overloading the functions S and C, we define the ODE
	# S'(s) = - [ iota(a+s,t+s) + omega(a+s,t+s) ] * S(s)
	#         + rho(a+s,t+s) * C(s)
	# C'(s) = - [ rho(a+s,t+s) + omega(a+s,t+s) + chi(a+s,t+s) ] * C(s)
	#         + i(a+s,t+s) * S(s)
	# with initial values
	# S(-a)  = pini(t-a)
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
	Sini = rate['pini'](t - a)
	Cini = 1.0 - Sini
	#
	# SC_ode
	SC_ode = scipy.integrate.ode(f, jac)
	SC_ode.set_integrator('vode')
	SC_ode.set_initial_value( [Sini, Cini], - a )
	#
	# integrate ODE from s = -a to s = 0
	SC = SC_ode.integrate(a)
	return SC
#
def sim_data(rate, integrand_name, bound, noise) :
	assert bound['age_lower'] == bound['age_upper']
	assert bound['time_lower'] == bound['time_upper']
	assert noise['meas_std'] == 0.0
	#
	# zero_fun
	def zero_fun(a, t) :
		return 0.0
	#
	# rate_extended
	rate_extended = dict()
	for key in ['iota', 'rho', 'chi', 'omega'] :
		if key in rate :
			rate_extended[key] = rate[key]
		else :
			rate_extended[key] = zero_fun
	#
	# iota, rho, chi, omega
	a     = bound['age_lower']
	t     = bound['time_lower']
	iota  =  rate_extended['iota'](a, t)
	rho   =   rate_extended['rho'](a, t)
	chi   =   rate_extended['chi'](a, t)
	omega = rate_extended['omega'](a, t)
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
	# avoid division by zero when omeage zero in cases above
	if integrand_name == 'relrisk' :
		return (omega + chi) / omega
	#
	# ODE cases
	(S, C) = SC_fun(a, t, rate_extended)
	P      = C / (S + C)
	integrand_value = {
		'susceptible' : S,
		'withC'       : C,
		'prevalence'  : P,
		'Tincidence'  : iota * (1 - P),
		'mtspecific'  : chi * P,
		'mtall'       : omega + chi * P,
		'mtstandard'  : (omega + chi) / (omega + chi * P)
	}
	return integrand_value[integrand_name]
