"""
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-23 Bradley M. Bell
# ----------------------------------------------------------------------------
{xrst_begin average_integrand}
{xrst_spell
   integrator
   integrators
   tol
   trapezoidal
}

Compute The Average Integrand
#############################

Syntax
******
{xrst_literal
   # BEGIN_AVERAGE_INTEGRAND
   # END_AVERAGE_INTEGRAND
}
{xrst_literal
   # BEGIN_RETURN
   # END_RETURN
}

Purpose
*******
The :ref:`predict_command-name` is a much faster way to calculate these values.
This routine is easier to use and provides independent testing of the
dismod_at integrators.

rate_fun
********
This is a dictionary and it's possible keys are the
:ref:`rate names<rate_table@rate_name>` .
If a *key* is a key in the dictionary,

   *value* = *rate_fun* [ *key* ]( *age* , *time* )

returns a float equal to the value of the specified rate
at the specified age and time (where *age* and *time* are floats).
Note that *age* will always be zero when evaluating

   *value* = *rate_fun* [ ``'pini'`` ]( *age* , *time* )

If a rate name is not in the *rate_fun*, the corresponding rate is zero.

integrand_name
**************
This string is one of the
:ref:`integrand names<integrand_table@integrand_name>` .

grid
****
This argument defines the grid for trapezoidal integration.
(The dismod_at integrands are often non-smooth and so a low order integration
method is called for.)
The *grid* is a dictionary with the following keys:

age
===
*grid* [ ``'age'`` ] is a list of floats containing the grid points
for the average w.r.t. age.
These points are monotone increasing, the first (last) point is the
lower (upper) age limit for the average

time
====
*grid* [ ``'time'`` ] is a list of floats containing the grid points
for the average w.r.t. time.
These points are monotone increasing, the first (last) point is the
lower (upper) time limit for the average

abs_tol
*******
This float is an absolute error bound, that the integrator will achieve.

avg
***
The return value *avg* is the corresponding
:ref:`average integrand<avg_integrand@Average Integrand, A_i>` .
{xrst_toc_hidden
   example/user/average_integrand.py
}

Example
*******
The file :ref:`user_average_integrand.py-name` contains an example and test of this routine.

{xrst_end average_integrand}
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
   Cini = rate['pini'](0.0, t - a)
   Sini = 1.0 - Cini
   #
   # Special case where scipy.integrate.ode chokes: a == 0.0
   iota  =  rate['iota'](a, t)
   rho   =   rate['rho'](a, t)
   chi   =   rate['chi'](a, t)
   omega = rate['omega'](a, t)
   if abs( 10.0 * max(iota, rho, chi, omega) * a ) < abs_tol :
      return [Sini, Cini]
   #
   # SC_ode
   # 2022-08-26: Tried to add rtol (relative tolerance) to this integrator
   # call and testing with example/user/average_integrand.py indicates
   # that it does not work (used different methods etc).
   SC_ode = scipy.integrate.ode(f, jac)
   SC_ode.set_integrator('lsoda', atol = abs_tol)
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
   msg  = 'average_integrand: ' + integrand_name
   msg += ' is not a valid integrand name'
   assert False, msg
#
# BEGIN_AVERAGE_INTEGRAND
# avg =
def average_integrand(rate_fun, integrand_name, grid, abs_tol) :
   assert type(rate_fun) == dict
   assert type(integrand_name) == str
   assert type(grid) == dict
   assert type(abs_tol) == float
   # END_AVERAGE_INTEGRAND
   #
   import scipy.integrate
   #
   # zero_fun
   def zero_fun(a, t) :
      return 0.0
   #
   # rate_extended
   rate_extended = dict()
   for key in ['pini', 'iota', 'rho', 'chi', 'omega'] :
      if key in rate_fun :
         rate_extended[key] = rate_fun[key]
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
   avg = float( trapezoidal_average_2d(func, age_grid, time_grid) )
   #
   # BEGIN_RETURN
   assert type(avg) == float
   return avg
   # END_RETURN
