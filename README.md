# osp_project

Dimitri VINET's OS And Programming repository.


# USAGE:

To use a .c file, execute the command:

    make <file name (without extension)>
    ex: make hello

This unmounts a directory if one with the same name exists and is mounted, deletes then recreates it, and (re-)mounts the new filesystem to it.

To clean a directory and its mounted filesystem, use:

    make clean_<dir name (without _d)>

### Use fuse to expose a single file always containing “Hello”

Use the file hello.c. When the filesystem is mounted, you can read from the file in hello_d with cat for example.

### ​ Use fuse to expose a single file that outputs what’s written to it in the console.

Use the file to_console.c. What's written on the file will be outputted on the original terminal where you executed make to_console.

### Use fuse to expose a single file that shuts down the computer when written to.

Use the file shutdown.c. Write to the file and the system will shut down.

### Use fuse to expose a file where you can write to and that saves what is written to it in an actual file somewhere on your disk.

Use the file write.c.

### Use fuse to expose a folder containing files: one to shutdown, one to suspend the computer when written to.

Use the file folder.c. Navigate through the directories and write to either shutdown or suspend to shutdown or suspend the computer.
    
### Expose a file that shows the results of the "sudo apt update" command when wanting to read from it.

Use the file update.c.
    
To show the results of "sudo apt update", open another terminal, and execute:

    cat /.../update_d/update

Exiting (with Ctrl + C for example) in the original terminal will unmount the filesystem.
    

    
hello: done

to_console: done

shutdown: done

write: DONE MF

folder: DONE

udpdate: done

