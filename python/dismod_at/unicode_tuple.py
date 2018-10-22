# $Id:$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# $begin unicode_tuple$$ $newlinech #$$
# $spell
#	iterable
#	unicode
#	tuple
#	str
#	dismod
#	sqlite
# $$
#
# $section Convert an Iterable Object to a Unicode String$$
#
# $head Syntax$$
# $icode%u_tuple% = dismod_at.unicode_tuple(%iterable%, %quote_string%)%$$
#
# $head iterable$$
# is any object where we can iterate over its elements
# and convert each element to a unicode value.
#
# $head quote_string$$
# is either $code True$$ or $code False$$.
# If it is $code True$$, each element of $icode iterable$$
# that is an $code str$$ or a $code unicode$$
# is surrounded by the single quote character in $icode u_tuple$$.
#
# $head None$$
# The value $code None$$ is converted to the unicode string $code null$$
# (always without quotes).
#
# $head Infinity$$
# If $icode u_j$$ is a $code float$$ and equal to plus infinity,
# the corresponding tuple value has an exponent that is ten times the
# exponent for the maximum float.
# This is so that, when written by pythons $code sqlite$$ package, it
# gets converted to infinity.
# Minus infinity is handled in a similar fashion.
#
# $head u_tuple$$
# is a unicode string representation of the tuple containing the elements.
# To be specific it is given by
# $codei%
#	%u_tuple% = u'( %u_1%, %u_2%, %...%, %u_n% )'
# %$$
# where $icode n$$ is the number of elements
# and $icode u_1$$ is the $th i$$ element.
#
# $children%example/table/unicode_tuple.py
# %$$
# $head Example$$
# The file $cref unicode_tuple.py$$ is an example use of
# $code unicode_tuple$$.
#
# $end
def unicode_infinity() :
	import sys
	import math
	exponent = int( math.log( sys.float_info.max ) / math.log(10.) ) + 10
	inf      = u'10e' + str(exponent)
	return inf
def unicode_tuple(iterable, quote_string) :
	u_tuple = u'('
	count   = 0
	for element in iterable :
		if count > 0 :
			u_tuple += u','
		count += 1
		#
		if element == None :
			u_tuple += u' null'
		elif isinstance(element, float) and element == float('inf') :
			u_tuple += u' ' + unicode_infinity()
		elif isinstance(element, float) and element == - float('inf') :
			u_tuple += u' -' + unicode_infinity()
		elif quote_string and isinstance(element, str) :
			u_tuple += u' ' + u"'" + str(element) + u"'"
		else :
			u_tuple += u' ' + str(element)
	u_tuple     += u' )'
	return u_tuple
