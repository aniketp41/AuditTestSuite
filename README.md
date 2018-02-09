# FreeBSD Audit TestSuite
Testsuite for Security Audit Framework In FreeBSD

## Explicit System Call Testing

The test application would trigger all Syscalls one by one, evaluating that the audit record contains all the expected parameters, e.g the arguments, valid argument types, return values etc. The testing will be done for various success and failure modes, with cross checking for appropriate error codes in case of failure mode.

## Workplan Report
* [Report1: Network System Call testing](https://gist.github.com/aniketp/4311599ab72efe73d8a3d3e1c93f3759)

## Directory Structure

Source contains following significant files
* **network.c** : Implementation of basic TCP socket which fires off a series of network syscalls. Each function is called twice, with the socket file descriptor being incorrect in one of the case, resulting in an expected error. Attempt is made to log both instances of each system call and then check whether the audit daemon logs them with the appropriate success and error message along with correct arguments.

* **setup** : A script to setup the environment. i.e, start the audit daemon in case its not already running and setting up the correct flag, `flags:lo,nt` in the file `audit_control`.

* **test** : A POSIX compliant shell script which does all the hard work. From firing off the network binary to extracting the data from resulting trail and analysing the result. Detailed functioning of the script is described later.

* **Makefile** : To compile and clean the resulting binaries

### Instructions
Current set of tests include the basic network socket system calls.

Clone the repository,
```bash
 $ git clone git@github.com:aniketp/AuditTestSuite.git audit
```

Setup the necessary values in configuration files
```bash
 $ ./setup
```

And execute the testing script.
```bash
 $ make
 $ ./test
```

### Testing Status

Current scenario: All the 8 included tests are passing in both *Success* and *Failure* modes.

|  Num  |	Syscall	 |  Status
|:-----:|:---------:|:-----------------:
1       |socket(2)	 	|:white_check_mark:
2       |bind(2)		|:white_check_mark:
3       |setsockopt(2)  |:white_check_mark:
4       |listen(2)      |:white_check_mark:
5       |accept(2)		|:white_check_mark:
6       |sendto(2)		|:white_check_mark:
7       |recvfrom(2)	|:white_check_mark:
8       |shutdown(2)	| (TBD)
9       |connect(2)     |:white_check_mark:
10      |sendmsg(2)     | (TBD)
11      |recvmsg(2)     | (TBD)