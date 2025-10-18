#! @python3_executable@
# $Id:$
# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# {xrst_begin python_main}
# {xrst_comment_ch #}
#
# The Python Main Program
# #######################
#
# Syntax
# ******
#
# dismod-at
# =========
# |tab| ``dismod-at``  database command ...
#
# dismodat.py
# ===========
# The following syntax does not work on windows and was
# Deprecated on 2025-05-06.
#
# |tab| ``dismodat.py`` database command ...
#
# command
# *******
# The commands that use the python main program ``dismod-at``
# (as apposed to the C++ main program ``dismod_at`` ) are:
#
# .. csv-table::
#     :widths: auto
#     :header: *command* , documentation
#
#     ``db2csv``   , :ref:`db2csv_command-title`
#     ``perturb``  , :ref:`perturb_command-title`
#     ``plot_rate_fit`` , :ref:`plot_rate_fit-title`
#     ``plot_data_fit`` , :ref:`plot_data_fit-title`
#
# Log Table
# *********
#
# Start
# =====
# This program adds a
# :ref:`log_table@message_type@command`
# log entry with at the start of the command.
# The :ref:`log_table@message` for this entry is
#
#     ``begin`` *command*  ...
#
# End
# ===
# If the command completes, a log entry with the following message is added
# to the log:
#
#     ``end`` *command*
#
# {xrst_end python_main}
# ---------------------------------------------------------------------------
def main():
   import sys
   import time
   import dismod_at
   #
   # check usage
   usage  = 'dismod-at database db2csv\n'
   usage += 'dismod-at database perturb tbl_name sigma\n'
   usage += 'dismod-at database plot_rate_fit pdf_file plot_title rate_set\n'
   usage += 'dismod-at database plot_data_fit pdf_file plot_title max_plot'
   #
   # deprecated
   # usage += 'dismod-at database csv2db configure_csv measure_csv\n'
   # usage += 'dismod-at database modify\\\n'
   # usage += '\ttable_name column_name row_expression value_expression\\\n'
   # usage += '\t[statement_one [statement_two ...] ]'
   #
   if len(sys.argv) < 3 :
      sys.exit(usage)
   #
   database_file_arg = sys.argv[1]
   command_arg       = sys.argv[2]
   arg_list          = sys.argv[3:]
   #
   # execute command
   if command_arg == 'db2csv' :
      if len(sys.argv) != 3 :
         sys.exit(usage)
      dismod_at.db2csv_command(database_file_arg)
   elif command_arg == 'perturb' :
      if len(sys.argv) != 5 :
         sys.exit(usage)
      tbl_name = arg_list[0]
      sigma    = arg_list[1]
      dismod_at.perturb_command(
         database_file_arg, tbl_name, sigma
      )
   elif command_arg == 'plot_rate_fit' :
      if len(sys.argv) != 6 :
         sys.exit(usage)
      pdf_file   = arg_list[0]
      plot_title = arg_list[1]
      rate_set   = set( arg_list[2].split() )
      dismod_at.plot_rate_fit(
         database_file_arg, pdf_file, plot_title, rate_set
      )
   elif command_arg == 'plot_data_fit' :
      if len(sys.argv) != 6 :
         sys.exit(usage)
      pdf_file   = arg_list[0]
      plot_title = arg_list[1]
      max_plot   = int( arg_list[2] )
      dismod_at.plot_data_fit(
         database_file_arg, pdf_file, plot_title, max_plot
      )
   elif command_arg == 'csv2db' :
      if len(sys.argv) != 5 :
         sys.exit(usage)
      configure_csv = sys.argv[3]
      measure_csv   = sys.argv[4]
      dismod_at.csv2db_command(database_file_arg, configure_csv, measure_csv)
   elif command_arg == 'modify' :
      # deprecated
      if len(sys.argv) < 7 :
         sys.exit(usage)
      dismod_at.modify_command(database_file_arg, arg_list)
   else :
      sys.exit(usage)
   #
   # ---------------------------------------------------------------------------
   program_name = sys.argv[0].replace('\\', '/').split('/')[-1]
   print( program_name + ': OK')
