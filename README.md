## Schmidt-Samoa Cryptography 
Schmidt-Samoa Cryptography implements the SS cryptosystem using C and the GMP integers library. I have completed this project as an assignment for CSE 13S in UCSC in Winter 2023. I have received instruction and help from several tutors and TA's. While their names are not listed within the code for privacy purposes, I have indicated what I received instruction and/or code on. This project involves generating public and private keys, encrypting a file using a public key, and decrypting the encrypted file using a private key. These utilize a mathematics library and an SS library that implements routines for SS.

## If you are a current CSE 13S student, please do not look at my source code!

## Build
There are several executables that can be created in this project, the main ones being 'keygen', 'encrypt', and 'decrypt'. Typing 'make' or 'make all' will build all of these executables and link all of the object files ('randstate.o', 'numtheory.o', and 'ss.o') necessary for those executables, within the 'asgn5' directory. Typing in 'make keygen', 'make encrypt' or 'make decrypt' will build those executable binary files and their linked object files individually.
 
## Cleaning
Type 'make clean' to remove the executable binary files 'keygen', 'encrypt', and 'decrypt', and all of the .o files from the Assignment 5 directory.

## Run Options
### Keygen
Running './keygen' followed by various command line options will yield public and private keys of user-specified composition. Typing in './keygen -h' will display command line options for keygen. Typing './keygen -b' followed by a number greater than or equal to 256 will create keys of the specified bit size. Otherwise, a key will be created by a default 256 bits. Typing './keygen -i' followed by a number will set the number of Miller-Rabin iterations for generating prime numbers. If no argument is specified, there will be a default of 50 iterations. Typing './keygen -n' followed by a file name will place the public key to that specified file. Otherwise, the public key would be placed in ss.pub. Typing './keygen -d' followed by a file name will place the private key to that specified file. Otherwise, the private key would be placed in ss.priv. Typing './keygen -s' followed by a number would set a random seed for testing. If no seed is provided, the seed would be the seconds since the UNIX epoch. Typing './keygen -v' will yield output of the username, and the bit size and decimal values of prime p, prime q, public key n, private exponent d, and private modulus pq.  

Note: Generate keys using keygen before using the encryptor and decryptor, as the keys generated will be used by them.

### Encrypt
Running './encrypt' followed by various command line options will encrypt a user's message using previously made public keys. Typing in './encrypt -h' will display command line options for encrypt. Typing './encrypt -i' followed by a file name will encrypt that file if found. Otherwise, the user can enter their message using standard input. Typing './encrypt -o' followed by a file name will return the encrypted message or file to an output file. Otherwise, the encrypted message will be outputted to standard output. Typing './encrypt -n' followed by a user specified public key file will ensure the encryptor uses the public key in that file. Otherwise if no argument is provided, ss.pub will be used. These public key files should have been generated by keygen. Typing './keygen -v' will yield output of the username and the bit size and decimal values of the public key n.

### Decrypt
Running './decrypt' followed by various command line options will decrypt a previously encrypted message and yield the original message. Typing in './decrypt -h' will display command line options for decrypt. Typing './decrypt -i' followed by a file name will decrypt that encrypted file if found. Otherwise, the user can enter the encrypted message using standard input. Typing './decrypt -o' followed by a file name will return the decrypted message or file to an output file. Otherwise, the decrypted message will be outputted to standard output. The encrypted files passed in should have been generated by the encryptor. Typing './decrypt -n' followed by a user specified private key file will ensure the decryptor uses the private key in that file. Otherwise if no argument is provided, ss.priv will be used. These public key files should have been generated by keygen. Typing './keygen -v' will yield output of the bit size and decimal values of the private modulus pq and private key d.
