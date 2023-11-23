#!/bin/bash

HEXTOOL=./bin2hex
RAW_IMAGE_FOLDER=../raw_images
OUTPUT_FILE=../clockDigit.h
FILE_TYPE=jpg
TMP_FILE=./tmp
# Make sure the original jpeg is less than 32Kbyes as there is an array size limit imposed by the Arduino IDE
FILE_SIZE_LIMIT=32

if [ ! -f "$HEXTOOL" ]; then
    echo
    echo "##################################"
    echo "Creating hex tool..."
    echo
    make && echo "Created hex tool" || echo "Create hex tool failed"
    echo "##################################"
fi

echo "#ifndef _IMAGE_CLOCK_DIGIT_H_" > $OUTPUT_FILE
echo "#define _IMAGE_CLOCK_DIGIT_H_" >> $OUTPUT_FILE
echo >> $OUTPUT_FILE
echo "#include <Arduino.h>" >> $OUTPUT_FILE
echo >> $OUTPUT_FILE

for file in $(ls $RAW_IMAGE_FOLDER/*.$FILE_TYPE)
do
    echo
    echo "Processing $file"

    file_size=$(du -k $file | cut -f1)
    # echo "File size: $file_size Kb"
    if [ "$file_size" -gt "$FILE_SIZE_LIMIT" ]; then
        echo "Error: File size exceeds $FILE_SIZE_LIMIT Kb. Please reduce the file size."
        [ -f $OUTPUT_FILE ] && rm $OUTPUT_FILE
        [ -f $TMP_FILE ] && rm $TMP_FILE
        exit
    fi

    $HEXTOOL --i $file --o $TMP_FILE
    
    filename=$(basename $file)
    filename=${filename%.*}
    echo "const uint8_t $filename[] PROGMEM = {" >> $OUTPUT_FILE
    cat $OUTPUT_FILE $TMP_FILE >> $OUTPUT_FILE 2> /dev/null
    echo "};" >> $OUTPUT_FILE
    echo >> $OUTPUT_FILE
done

echo "#endif" >> $OUTPUT_FILE

[ -f $TMP_FILE ] && rm $TMP_FILE