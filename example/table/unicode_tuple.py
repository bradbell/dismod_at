# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# -------------------------------------------------------------------------- */
# $begin unicode_tuple.py$$ $newlinech #$$
# $spell
#	unicode_tuple
# $$
#
# $section unicode_tuple: Example and Test$$
#
# $srcthisfile%0%# BEGIN PYTHON%# END PYTHON%1%$$
# $end
# BEGIN PYTHON
#
def unicode_tuple() :
	import dismod_at
	#
	iterable     = [ 1, 2.00, 'three' ]
	quote_string = True
	u_tuple      = dismod_at.unicode_tuple(iterable, quote_string)
	check        = u'( 1, 2.0, \'three\' )'
	assert u_tuple == check
	#
	print('unicode_tuple: OK')
# END PYTHON
