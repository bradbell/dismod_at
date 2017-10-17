# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-17 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin user_metropolis.py$$ $newlinech #$$
# $spell
#	init
# $$
#
# $section Predict Average Integrand Using Results of a Fit$$
#
# $code
# $srcfile%
#	example/user/metropolis.py
#	%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $$
# $end
# ---------------------------------------------------------------------------
iota_north_america = 1e-2
# ---------------------------------------------------------------------------
# BEGIN PYTHON
import sys
import os
test_program = 'example/user/metropolis.py'
if sys.argv[0] != test_program  or len(sys.argv) != 1 :
	usage  = 'python3 ' + test_program + '\n'
	usage += 'where python3 is the python 3 program on your system\n'
	usage += 'and working directory is the dismod_at distribution directory\n'
	sys.exit(usage)
print(test_program)
#
# import dismod_at
local_dir = os.getcwd() + '/python'
if( os.path.isdir( local_dir + '/dismod_at' ) ) :
	sys.path.insert(0, local_dir)
import dismod_at
#
import scipy.stats
import numpy
import time
# -----------------------------------------------------------------------------
# Simulate data with and N(2, 0.5^2) distribution
python_seed = int( time.time() )
numpy.random.seed(seed = python_seed)
N          = 1000
mu_true    = 2.0
sigma_true = 0.5
y          = scipy.stats.norm.rvs(loc=mu_true, scale=sigma_true, size= N)
# -----------------------------------------------------------------------------
# model the data as N(mu, sigma) where x = (mu, sigma) is unknown
minus_infinity = - float("inf")
def log_f(x) :
	mu    = x[0]
	sigma = x[1]
	if sigma <= 0 :
		return minus_infinity
	return sum( scipy.stats.norm.logpdf(y, loc=mu, scale=sigma) )
# -----------------------------------------------------------------------------
# run the metropolis algorithm to estimate (mu, sigma)
m   = 2000                 # length of the chain
x0  = numpy.array([1, 1])  # start at mu = 1, sigma = 1
s   = 0.03                 # scale factor for proposal random walk
(a, c) = dismod_at.metropolis(log_f, m, x0, s)
# -----------------------------------------------------------------------------
# compute and check estimates for mu and sigma
burn_in        = int( 0.1 * m )
c              = c[burn_in :, :]
x_estimate     = numpy.average(c, axis=0)
mu_estimate    = x_estimate[0]
sigma_estimate = x_estimate[1]
#
relerr = abs( mu_estimate / mu_true - 1.0 )
if relerr > 0.05 :
	print('relerr = ', relerr, ", python_seed = ", python_seed)
	assert(False)
relerr = abs( sigma_estimate / sigma_true - 1.0 )
if relerr > 0.05 :
	print('relerr = ', relerr, ", python_seed = ", python_seed)
	assert(False)
# -----------------------------------------------------------------------------
print('metropolis.py: OK')
# END PYTHON
