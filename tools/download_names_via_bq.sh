#!/bin/bash

set -o errexit

get_names() {
  echo "------------------------------------$1------------------------------------"
  bq query --destination_table=caps.results_$1 --use_legacy_sql=false "SELECT DISTINCT name FROM (SELECT p443.https.tls.certificate.parsed.names AS names FROM \`censys-io.ipv4_public.$1\`), UNNEST(names) AS name"
  bq extract --destination_format=CSV caps.results_$1 gs://caps-domain-names/$1.csv
}

for line in `cat $1`; do
  get_names $line
done
