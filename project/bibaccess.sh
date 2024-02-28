#!/bin/bash

# Usage information
usage() {
    echo "Usage:"
    echo -e "\t./bibaccess --query log1 ... logN"
    echo -e "\tor"
    echo -e "\t./bibaccess --loan log1 ... logN"
    exit 1
}

# Check arguments
if [[$# -lt 3 || ($1 != "--query" && $1 != "--loan")]]; then
    usage
fi

# Initialize variables for tot query/loan
total_query=0
total_loan=0

# Loop through arguments starting from the second one
for ((i=2; i<=$#; i++)); do
  logfile="${!i}"  # Get the ith argument as logfile
  if [ ! -f "$logfile" ]; then
    echo "$logfile is not a valid file."
    exit 1
  fi

  # Process the logfile based on the option
  case $1 in
    "--query")
      queries=$(grep -c "QUERY" "$logfile")
      echo "$logfile $queries"
      total_queries=$((total_queries + queries))
      ;;
    "--loan")
      loans=$(grep -c "LOAN" "$logfile")
      echo "$logfile $loans"
      total_loans=$((total_loans + loans))
      ;;
  esac
done

# Print total queries or loans depending on the option
case $1 in
  "--query")
    echo "QUERY $total_queries"
    ;;
  "--loan")
    echo "LOAN $total_loans"
    ;;
esac

exit 0