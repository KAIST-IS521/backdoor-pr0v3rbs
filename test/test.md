# test.mini description

First, this codes test heap address by making `"Pass"` string on 0x0 address of heap.
When making string this codes also test `load`, `store`, `add`, `puti` opcodes.

And then, is codes test `gt`, `ge`, `eq` opcodes by using `ite` opcode.
When test compare opcodes this codes will test integer overflow, integer underflow and byte overflow.

Next, test `gets`, `puts` opcodes.
Program will wait user input, and when user inputs some string, then program will print `"Pass" + userinput` string.

Last, test `jump`, `halt` opcodes.

```C
// heap address test
heap = malloc(8192);
r10 = 0;
// 'store', 'puti', 'add' test
heap[r10++] = 'P';
heap[r10++] = 'a';
heap[r10] = 's';
// 'load' test
r5 = heap[r10++]
heap[r10] = r5;
r10 = 0;

// 'gt', 'ge', 'eq' test by testing 'ite' opcode.
r0 = 0;
r1 = 0;
r2 = 1;
// 'gt' test
r0 -= r2; // underflow test, r0 = 0xffffffff, r1 = 0x0
if ((r0 > r1) == false) exit(1)

r0 += r2; // overflow test, r0 = 0x0, r1 = 0x0
if ((r0 > r1) == true) exit(1)

r1 += r2; // overflow test, r0 = 0x0, r1 = 0x1
if ((r0 > r1) == true) exit(1)

// 'ge' test
// r0 = 0x0, r1 = 0x1
if ((r0 >= r1) == true) exit(1)

r1 -= r2; // r0 = 0x0, r1 = 0x0
if ((r0 >= r1) == false) exit(1)

r0 -= r2; // r0 = 0xffffffff, r1 = 0x0
if ((r0 >= r1) == false) exit(1)

// 'eq' test
r0 += r2; // r0 = 0x0, r1 = 0x0
if ((r0 == r1) == false) exit(1)

r1 = 0x80 + 0x80; // byte overflow test, r0 = 0x100, r1 = 0x0
if ((r0 == r1) == true) exit(1)

// 'gets', 'puts' opcodes test
r0 = 10;
gets(&heap[r0]);   // expected result - waiting user input
printf("%s", r10); // expected result - print "Pass"
printf("%s", r0);  // expected result - print user input

// jump test
:loop
goto halt
goto loop
:halt
// expected result - successfully exit
exit(0)
```
