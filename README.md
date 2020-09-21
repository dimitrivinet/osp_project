# osp_project

Dimitri VINET's OS And Programming repository.


# USAGE:

To use a .c file, execute the command:

    make <file name (without extension)>
    ex: make hello

This unmounts a directory if one with the same name exists and is mounted, deletes then recreates it, and (re-)mounts the new filesystem to it.

To clean a directory and its mounted filesystem, use:

    make clean_<dir name (without _d)>
    
### Expose a file that shows the results of the "sudo apt update" command when wanting to read from it.

To expose this file, execute the command:
    
    make update
    
To show the results of "sudo apt update", open another terminal, and execute:

    cat /.../update_d/update
    

    
hello: done

to_console: done

shutdown: done

write: not begun

folder: not begun

udpdate: done

