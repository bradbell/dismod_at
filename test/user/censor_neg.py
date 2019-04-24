# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-19 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Under Construction: see omh/math/censor_neg.omh
# ---------------------------------------------------------------------------
random_seed   = 0
n_data        = 1000
mu_true       = 0.5
delta_true    = 1.0
distribution  = 'gaussian'
# ---------------------------------------------------------------------------
import sys
import os
import distutils.dir_util
import scipy.stats
import numpy
import math
test_program = 'test/user/censor_neg.py'
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
# change into the build/test/user directory
distutils.dir_util.mkpath('build/test/user')
os.chdir('build/test/user')
# -----------------------------------------------------------------------------
import time
if random_seed == 0 :
	random_seed = int( time.time() )
numpy.random.seed(random_seed)
# -----------------------------------------------------------------------------
def simulate_censored_gaussian(mu, delta) :
	loc   = mu
	scale = delta
	y     = scipy.stats.norm.rvs(mu, delta)
	return  max(y, 0)
#
def simulate_censored_laplace(mu, delta) :
	loc   = mu
	scale = delta / math.sqrt(2.0)
	y     = scipy.stats.laplace.rvs(loc, scale)
	return  max(y, 0)
#
def loglike_censored_gaussian(y, mu, delta ):
	if y == 0.0 :
		denominator = math.sqrt(2.0) * delta
		return log( scipy.special.erfc(mu / denominator) / 2.0 )
	loc   = mu
	scale = delta
	return scipy.stats.norm.logpdf(y, loc, scale)
#
def loglike_censored_laplace(y, mu, delta ):
	if y <= 0.0 :
		- mu * math.sqrt(2.0) / delta - math.log(2.0)
	loc   = mu
	scale = delta / math.sqrt(2.0)
	return scipy.stats.laplace.logpdf(y, loc, scale)
# -----------------------------------------------------------------------------
# simulate data
data = numpy.zeros(n_data)
if distribution == 'gaussian' :
	simulate = simulate_censored_gaussian
	loglike  = loglike_censored_laplace
elif distribution == 'laplace' :
	simulate = simulate_censored_laplace
	loglike  = loglike_censored_laplace
else :
	assert False
#
# simulate
count = 0
for i in range(n_data) :
	data[i] = simulate(mu_true, delta_true)
	if data[i] == 0.0 :
		count += 1
assert count > n_data / 10.0
#
# objective
def negloglike(mu) :
	ret = 0.0
	for i in range(n_data) :
		ret -= loglike(data[i], mu, delta_true)
	return ret
#
bounds = [0.0 , 2.0 * mu_true]
method = 'bounded'
disp   = 0 # 0, 1, 2, or 3: 0 least printing, 3 most
result = scipy.optimize.minimize_scalar(
	negloglike,
	bounds=[0.0 , 10.0 * mu_true],
	method='bounded',
	options={'disp':disp}
)
mu_fit  = result.x
rel_err = 1.0 - mu_fit / mu_true
if abs(rel_err) > 1e-1 :
	print("rel_err = ",     relerr)
	print("random_seed = ", random_seed)
# -----------------------------------------------------------------------------
print('censor_neg.py: OK')
