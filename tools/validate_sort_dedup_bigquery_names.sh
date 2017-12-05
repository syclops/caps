# Validate, sort, and deduplicate a CSV file of domain names downloaded from Censys through
#
# Author: Steve Matsumoto

# Exit if an error occurs during the execution of the script.
set -o errexit

# Exit if the script is called without exactly one .lz4 file as an argument.
if [[ $# -ne 1 || $1 != *.csv ]]; then
  echo "You must pass exactly one argument of the form '.csv'."
  exit 1
fi

PYTHONEXEC=python3

# Set the appropriate file names.
TOOLS_DIR=$(dirname $0)
BASE_DIR=$(dirname $1)
BASENAME=$BASE_DIR/$(basename $1 .csv)
OUTFILE=$BASENAME.dns.rev.sorted.txt

# Print all variable names for debugging purposes.
#echo $PYTHONEXEC
#echo $TOOLS_DIR
#echo $BASENAME
#echo $LOGFILE
#echo $OUTFILE
#echo $DATAFILE

echo "Validating, sorting, and deduplicating DNS names from $1"
time $PYTHONEXEC $TOOLS_DIR/validate_names.py $1 | sort | uniq > $OUTFILE
