# Assignment2: String Manipulation

## Build
```sh
$ gcc209 -o rect rect.c
```

## Testing (Example)
```sh
./samplewc209 < ./tests/input/sample01.txt > output1 2> errors1
./main < ./tests/input/sample01.txt > output2 2> errors2
diff output1 output2
diff errors1 errors2
rm output1 output2 errors1 errors2

./samplewc209 < ./tests/input/samplej03.txt > output1 2> errors1
./wc209 < ./tests/input/samplej03.txt > output2 2> errors2
diff output1 output2
diff errors1 errors2
rm output1 output2 errors1 errors2

$ ./main < ./tests/input/sample01.txt >stdout 2>stderr
$ diff ./tests/answer/sample00.stdout stdout
$ diff ./tests/answer/sample00.stderr stderr

$ ./rect < ./tests/input/sample01.txt >stdout 2>stderr
$ diff ./tests/answer/sample01.stdout stdout
$ diff ./tests/answer/sample01.stderr stderr

$ ./rect < ./tests/input/sample02.txt >stdout 2>stderr
$ diff ./tests/answer/sample02.stdout stdout
$ diff ./tests/answer/sample02.stderr stderr

$ ./rect < ./tests/input/sample03.txt >stdout 2>stderr
$ diff ./tests/answer/sample03.stdout stdout
$ diff ./tests/answer/sample03.stderr stderr
```

You can also compare yours with `samplerect`.

## Submission
1. Make `readme` and put `EthicsOath.pdf` to the current directory.
2. Change your `STUDENT_ID` with yours.
3. If your ID is 20221234, then
```sh
$ mkdir 20221234_assign0
$ mv rect.c readme EthicsOath.pdf 20221234_assign0
$ cd 20221234_assign0
$ tar -zcf 20221234_assign0.tar.gz *
```
4. Check your submission using a given script.
```
$ ./tools/check.py 20221234_assign0.tar.gz
[+] Good to submit :)
```
