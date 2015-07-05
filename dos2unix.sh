#! /bin/sh

makes=`find ./ -name "*.am" -type f`
header=`find ./ -name "*.h" -type f`
source=`find ./ -name "*.cpp" -type f`

for file in $makes
do
    dos2unix $file
done

for file in $header
do
    dos2unix $file
done

for file in $source
do
    dos2unix $file
done

echo "######### Done! #########"
