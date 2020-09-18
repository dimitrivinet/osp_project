# osp_project

Dimitri VINET's OS And Programming repository.


# USAGE:

To use a .c file, execute the command:\n
    make <file name (without extension)>
    ex: 
    '''
    make hello
    '''
    
This unmounts a directory if one with the same name exists and is mounted, deletes then recreates it, and (re-)mounts the new filesystem to it.

To clean all directories and their mounts, use:
    '''
    make clean
    '''
