# Assignment2: String Manipulation

## Build
```sh
$ make
```
gcc209 -o sgrep sgrep.c str.c

## Testing (Example)
```sh
# Part 1: The Basic Str Function Implementation
$ ./client StrCopy

# Part 2: Simple Grep
```sh
./tests/input/find01.sh >stdout 2>stderr
diff ./tests/answer/find01.stdout stdout
diff ./tests/answer/find02.stderr stderr

./tests/input/find02.sh >stdout 2>stderr
diff ./tests/answer/find01.stdout stdout
diff ./tests/answer/find02.stderr stderr

./tests/input/find03.sh >stdout 2>stderr
diff ./tests/answer/find01.stdout stdout
diff ./tests/answer/find02.stderr stderr

./tests/input/find08.sh >stdout 2>stderr
diff ./tests/answer/find01.stdout stdout
diff ./tests/answer/find02.stderr stderr

./tests/input/find09.sh >stdout 2>stderr
diff ./tests/answer/find01.stdout stdout
diff ./tests/answer/find02.stderr stderr
```

## Submission
1. Make `readme` and put `EthicsOath.pdf` to the current directory.
2. Change your `STUDENT_ID` with yours.
3. Run 
```sh
$ make submit
```
4. Submit your `$(STUDENT_ID)_assign2.tar.gz`.
