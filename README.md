# scopy
Copy files or directories filtered with multi exclude or include regular expression pattern

### Usage
scopy source [destination] [options]...

    source              source file or directory, must exist
    destination         destination file or directory, see note

options:

    -h                  help
    -e <regex>          exclude filter, see note
    -i <regex>          include filter, see note
    -t                  tolerant, continue when error ocurrs
    -u                  update, overwrite only if file modification time or size is different
    -v                  verbose

### Note
1. The order of source, destination and options make no sense except that source should be in
front of destination.
2. If destination dosn't exist:
    1. If source is a directory, destination is always treated as a  directory
    2. If source is a file, append a \ or / to destination will make scopy consider it's a
       directory, otherwise a file.
3. Patterns is apply to every file path in the source path, absolutely or relatively as given.
Default patter is include all files. For every file if none exclude pattern matched, then it's
included, else continue check if any include pattern matched, then it's included, else it's 
excluded. In other works include filter has high priority than exclude filter.

### Example
scopy dir1 dir2 -e regex1 -i regex2 -v -t -e regex3 ...

### Build
Windows exe and Visual C++ 2017 project file provided.
Other platform with C++17 compiler may supported, just haven't test it. You can do it yourself
since there is only one .cpp file with 100+ lines of code in it and dosn't use any external 
libaray or system API except standard C++17 feature.

https://github.com/byteatom/scopy