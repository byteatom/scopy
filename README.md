# scopy
Copy files or directories filtered with multi ordered include or exclude regular expression

usage: scopy source [destination] [options]...

    source              source file or directory
    destination         destination file or directory

options:
    -e <regex>          exclude filter
    -i <regex>          include filter
    -t                  tolerant, continue when error ocurrs
    -u                  update, overwrite only if file modification time or size is different
    -v                  verbose

note: If source is a file and destination dosn't exist, if destination end with '\' or '/'
    means it's a directory, otherwise it's a file.

example: scopy dir1 dir2 -e regex1 -i regex2 -e regex3 ...

# build
C++17 compiler needed