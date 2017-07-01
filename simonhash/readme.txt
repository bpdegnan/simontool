simonhash.sh

This BASH script uses the simontool program to explore the use of the Simon Cipher key schedule as the basis for a hash function.  This is achieved using the "bashbignumbers.sh" library and the simontool program with the -y and -u options.  The -y option prints the key schedule, and the -u option is used to change the key schedule behavior.  As an example:
    simontool -e -b 32 -k 64 -s 0000000000000000  -t 00000000 -y -u | tail -2 | head -1
Will output
    final key: 549b 6ca9 3bfa fb04
    
By changing the "-s <ARGUMENT>" you can chain the data into the key schedule.

In order to automate the exploration of the Simon Cipher as a hash, the simonhash.sh script is a wrapper for simontool.

The options are
-a  use the AND key schedule logic
-c  use a counter
-k  <key size> to configure key size

As an example, the following line shows running the 96-bit configuration of the key schedule with the AND logic, the counter, on files unix, zero1M and random1M:
simonhash.sh -a -c -k 96 unix zero1M random1M

The hash results can then be analyzed via
analyzehash.sh unix.96.48.1.1.hash



