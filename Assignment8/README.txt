Question f:

- Run the repository.c file
- Open two other terminals and run the writer.c program on each of them.
- The one last run will wait to get the access of the resource.
- It will get the access of the resource after the first program is done writing and signals the resource

Question g:
- Run the repository.c file
- Open two other terminals and run the writer.c program on one of them.
- Then, run the reader.c program in the other
- The reader waits to get the access of the resource.
- It will get the access of the resource after the writer is done writing and signals the resource

Question h:
- Run the repository.c file
- Open two other terminals and run the reader.c program on one of them.
- Both read simultaneously

Important
- Do create a 'shmfile' before running any of the files. It is required to make keys. Also, please change the sleep time in
the repository.c as per your convenience.

