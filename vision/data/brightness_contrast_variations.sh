#!/bin/bash

FILE=
INPUT_DIR=
OUTPUT_DIR=/tmp/output
VARIANTS=10
#BUCKETS=100

fail() {
  echo "Error:" $1  >&2
  exit 1;
}

parse_args() {
  TEMP=`getopt -o d:o:n: -n '$0' .. "$@"`
  if [ $? != 0 ] ; then fail "invalid arguments" >&2 ; fi
  
  while true ; do
          case "$1" in
            -d) INPUT_DIR=$2 ; echo INPUT_DIR=$INPUT_DIR; shift 2 ;;
            -o) OUTPUT_DIR=$2 ; echo OUTPUT_DIR=$OUTPUT_DIR ; shift 2 ;;
            -n) VARIANTS=$2 ; echo VARIANTS=$VARIANTS ; shift 2 ;;
             *) case "$1" in
                  "")
                     if [ "$INPUT_DIR" == "" ] ; then fail "missing input file or directory" ; fi
                     break
                     ;;
                  *)
                     if [ "$FILE" != "" ] ; then fail "illegal argument '$1'" ; fi
                     if [ "$INPUT_DIR" != "" ] ; then fail "specified both input file and directory" ; fi
                     FILE=$1
                     shift
                     ;;
                esac ;;
          esac
  done
}

choose_file() {
  echo $(find $1 -type f | shuf -n 1)
}

rand() {
  min=$1
  max=$2
  ((range = 1 + max - min))
  ((result = min + (RANDOM * range / 32767)))
  echo $result
}

generate_sample() {
  INFILE=$1
  OUTFILE=$2.png
  brightness=$(rand "-30" "30")
  contrast=$(rand "-30" "30")
  rotate=$(rand 0 360)
  echo $INFILE brightness = $brightness contrast = $contrast rotate = $rotate $OUTFILE
}

parse_args $*

[ -f $FILE ] && [ -r $FILE ] || fail "Cannot acccess $1"
[ -d $OUTPUT_DIR ] && [ -w $OUTPUT_DIR ] || echo "Create output directory $OUTPUT_DIR" && mkdir -p $OUTPUT_DIR || fail "Could not create output directory $2"
echo "Generating $VARIANTS variants of $FILE in $OUTPUT_DIR"

count=0
while [ "$count" -lt $VARIANTS ]
do
  #val=$(rand -0 $BUCKETS)
  #((hits[$val]++))
  #generate_sample $(choose_file $INPUT_DIR) $count
  generate_sample $(choose_file $INPUT_DIR) $count
  ((count++))
done
echo "-----------------"

#count=0
#while [ "$count" -le $BUCKETS ]
#do
#  echo $count "    " ${hits[$count]}
#  ((count++))
#done