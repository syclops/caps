# Extract reversed DNS names (i.e., com.example.www) from a lz4-compressed JSON
# file from the 443-https-tls-full_ipv4 data provided by Censys.
#
# Author: Steve Matsumoto

# Exit if an error occurs during the execution of the script.
set -o errexit

# Exit if the script is called without exactly one .lz4 file as an argument.
if [[ $# -ne 1 || $1 != *.lz4 ]]; then
  echo "You must pass exactly one argument of the form '.lz4'."
  exit 1
fi

# Test for lz4 and exit if it is not available.
if ! command -v lz4 > /dev/null; then
  echo "You must have lz4 installed."
  exit 1
fi

# Set the Python executable to pypy3 or python3 depending on availability.
# otherwise.
#if command -v pypy3 > /dev/null; then
  #PYTHONEXEC=pypy3
#elif command -v python3 > /dev/null; then
  #PYTHONEXEC=python3
#else
  #echo "You must have python3 or pypy3 installed."
  #exit 1
#fi
PYTHONEXEC=python3

# Set the appropriate file names.
TOOLS_DIR=$(dirname $0)
BASE_DIR=$(dirname $1)
BASENAME=$BASE_DIR/$(basename $1 .lz4)
LOGFILE=$BASENAME.dns.log
OUTFILE=$BASENAME.dns.rev.sorted.txt

# Print all variable names for debugging purposes.
#echo $PYTHONEXEC
#echo $TOOLS_DIR
#echo $BASENAME
#echo $LOGFILE
#echo $OUTFILE
#echo $DATAFILE

# Extract the Censys lz4 file.
if [[ ! -f $BASENAME ]]; then
  echo "Decompressing lz4 file."
  lz4 $1
else
  echo "$BASENAME already exists. Skipping lz4 decompression."
fi

# Extract the domain names.
echo "Extracting DNS names from $BASENAME"
time $PYTHONEXEC $TOOLS_DIR/get_dns_names.py --ip --reverse $BASENAME $LOGFILE $OUTFILE
rm -f $BASENAME
