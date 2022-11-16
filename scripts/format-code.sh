#/bin/bash

echo "Retrieving files to format..."
FILES=$(git status -s | awk '{ $1=""; print substr($0,2) }')

for I in $FILES
do
    echo "Formatting ${I}"
    clang-format -i ${I}
done

echo "Done."
