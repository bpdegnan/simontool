simonhash.sh

This BASH script uses the simontool program to explore the use of the Simon Cipher key schedule as the basis for a hash function.  This is achieved using the "bashbignumbers.sh" library and the simontool program with the -y option.  The -y option prints the key schedule.  As an example:
    simontool -e -b 32 -k 64 -s 0000000000000000  -t 00000000 -y | tail -2 | head -1
Will output
    final key: 549b 6ca9 3bfa fb04
    
By changing the "-s <ARGUMENT>" you can chain the data into the key schedule.

