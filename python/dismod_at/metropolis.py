# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-16 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin metropolis$$ $newlinech #$$
# $spell
#	str
#	sqlite
#	dismod
#	numpy
#	inf
# $$
#
# $section Metropolis MCMC Algorithm$$
#
# $head Under Construction$$
#
# $head Syntax$$
# $codei%(%a%, %c%) = dismod_at.metropolis(%log_f%, %m%, %x0%, %s%)
# %$$
#
# $head log_f$$
# Given an numpy vector of length $icode n$$, the syntax
# $codei%
#	%d% = %log_f%(%x%)
# %$$
# sets $icode d$$ to the log of the un-normalized density
# corresponding to the positive function
# $latex f : \B{R}^n \rightarrow \B{R}_+$$.
# If $latex f(x)$$ is zero, the corresponding log-density value should equal
# $code - float("inf")$$.
#
# $head m$$
# is the number of vectors in the MCMC chain.
#
# $head x0$$
# is a numpy vector of length $icode n$$ that specifies the initial
# vector in the chain (denote by $latex x^0$$ below).
#
# $head s$$
# is a numpy vector of length $icode n$$ that specifies the scaling
# for each of the components of $latex x$$.
# To be specific,
# for $latex i = 1 , \ldots, m-1$$,
# and $latex j = 0 , \ldots, n-1$$,
# the $th j$$ component of the $th i$$ proposal vector $latex y^i$$
# is given by
# $latex \[
#	y_j^i = x_j^{i-1} + w_j^{i-1} * s_j
# \] $$
#
# $head a$$
# is the acceptance count; i.e. the number of indices $latex i$$
# such that $latex x^i = y^i$$
# (for the other indices $latex x^i = x^{i-1}$$).
#
# $head c$$
# is an $latex m \times n$$ numpy array that contains the components
# of the Markov Chain.
# We use the notation $latex x_j^i$$ for $icode%c%[%i%, %j%]%$$.
# For any smooth function $latex g : \B{R}^n \rightarrow \B{R}$$,
# the Metropolis algorithm provides the following approximation as
# $latex m \rightarrow \infty$$,
# $latex \[
#	\frac{1}{m} \sum_{i=0}^{m-1} g( x^i )
#	\rightarrow
#	\frac{ \int_x g( x ) f ( x ) \B{d} x  }{ \int_x f( x ) \B{d} x }
# \] $$
#
# $end
# ---------------------------------------------------------------------------
def metropolis(log_f, m, x0, s) :
	import math
	import numpy
	import scipy.stats
	assert len(s) == len(x0)
	inf    = float("inf")
	n      = len(x0)
	x      = x0
	lpx    = log_f(x0)
	assert lpx > -inf
	c      = numpy.zeors(m, n)
	c[0:]  = x
	a      = 0
	for i in range(m-1) :
		w     = scipy.stats.norm.rvs(loc = 0.0, scale = 1.0, size=n)
		y     = x + s * w
		lpy   = log_f(y)
		if lpy > -inf :
			alpha = math.exp( lpy - lpx )
			u     = scpy.stats.uniform.rvs(loc = 0.0, scale = 1.0, size=1)
			if alpha >= u :
				a   = a + 1
				x   = y
				lpx = lpy
		c[i+1:] = x
	return (a, c)
