# compiling
gcc 210101093_q1b.c -o 210101093-q1b
# unsetting environment variables
unset -v "n"
# setting environment variables if given in command line
if [ "$#" -gt 0 ]; then
    export n="$@"
fi
# running the file
./210101093-q1b 

# Command lines
# sudo chmod 777 compile-1b-and-execute.sh
# ./compile-1b-and-execute.sh (give the environment variable value here )

# example
# sudo chmod 777 compile-1b-and-execute.sh
# ./compile-1b-and-execute.sh 10 20