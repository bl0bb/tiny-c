# Assembly Documentation


## Terminology
<p>In assembly, its common to see certain words in the assembly code that represent size, these are the different sizes:</p>

|              | BYTE | WORD | DWORD (double word) | QWORD (quad word) |
| ------------ | ---- | ---- | ------------------- | ----------------- |
| Size (bytes) | 1    | 2    | 4                   | 8                 |
| Size (bits)  | 8    | 16   | 32                  | 64                |

<p>Keep in mind that certain CPU architectures, operating systems, and environments treat some of these values differently</p>



## Registers

### General purpose
| 64 bit | 32 bit | 16 bit | 8 bit high | 8 bit low | usage / common usage                                         | Callee saved |
| ------ | ------ | ------ | ---------- | --------- | ------------------------------------------------------------ | ------------ |
| RAX    | EAX    | AX     | AH         | AL        | Return value, Accumulator (arithmetic, I/O, multiply/divide) | No           |
| RBX    | EBX    | BX     | BH         | BL        | General purpose, Base register (memory addressing)           | Yes          |
| RCX    | ECX    | CX     | CH         | CL        | 4th integer argument, Counter register (loops, shifts)       | No           |
| RDX    | EDX    | DX     | DH         | DL        | 3rd integer argument, Data register (I/O, multiply/divide)   | No           |

### String / memory manipulation
| 64 bit | 32 bit | 16 bit | 8 bit high | 8 bit low | usage / common usage                                        | Callee saved |
| ------ | ------ | ------ | ---------- | --------- | ----------------------------------------------------------- | ------------ |
| RSI    | ESI    | SI     | -          | -         | 2nd integer argument, Source index (string/memory ops)      | No           |
| RDI    | EDI    | DI     | -          | -         | 1st integer argument, Destination index (string/memory ops) | No           |

### Stack
| 64 bit | 32 bit | 16 bit | 8 bit high | 8 bit low | usage / common usage        | Callee saved                        |
| ------ | ------ | ------ | ---------- | --------- | --------------------------- | ----------------------------------- |
| RBP    | EBP    | BP     | -          | -         | Base pointer (stack frames) | Yes                                 |
| RSP    | ESP    | SP     | -          | -         | Stack pointer               | Yes (implicitly - must be restored) |

### Extended general purpose
| 64 bit | 32 bit | 16 bit | 8 bit high | 8 bit low | usage / common usage                  | Callee saved |
| ------ | ------ | ------ | ---------- | --------- | ------------------------------------- | ------------ |
| R8     | R8D    | R8W    | -          | R8B       | 5th integer argument, General-purpose | No           |
| R9     | R9D    | R9W    | -          | R9B       | 6th integer argument, General-purpose | No           |
| R10    | R10D   | R10W   | -          | R10B      | General-purpose                       | No           |
| R11    | R11D   | R11W   | -          | R11B      | General-purpose                       | No           |
| R12    | R12D   | R12W   | -          | R12B      | General-purpose                       | Yes          |
| R13    | R13D   | R13W   | -          | R13B      | General-purpose                       | Yes          |
| R14    | R14D   | R14W   | -          | R14B      | General-purpose                       | Yes          |
| R15    | R15D   | R15W   | -          | R15B      | General-purpose                       | Yes          |

### Other
| 64 bit | 32 bit | 16 bit | 8 bit high | 8 bit low | usage / common usage                                                                   | Callee saved |
| ------ | ------ | ------ | ---------- | --------- | -------------------------------------------------------------------------------------- | ------------ |
| RIP    | EIP    | IP     | -          | -         | Instruction pointer (or Return Instruction Pointer), holds address of next instruction | N/A          |



## Segment Registers
| Register | Name            | Primary purpose                                                | Used in modern computers |
| -------- | --------------- | -------------------------------------------------------------- | ------------------------ |
| CS       | Code Segment    | Points to the segment containing instructions (program code).  | No                       |
| DS       | Data Segment    | Default segment for most non-stack data loads/stores.          | No                       |
| SS       | Stack segment   | Used for stack operations (push/pop, call/ret).                | No                       |
| ES       | Extra Segment   | Historically used as an additional data segment.               | No                       |
| FS       | Extra Segment F | Used for special data structures (e.g., thread local storage). | Yes                      |
| GS       | Extra Segment G | Often used for OS-specific structures (e.g., CPU local data).  | Yes                      |







## Patterns in all registers:
<p>
"base name" of a register (e.g. AX) is the 16 bit version<br>
add an "E" at the start and you get the 32 bit version<br>
add an "R" at the start and you get the 64 bit version
</p>

## For general purpose registers:
<p>
there are 4 general purpose registers: A, B, C, D. All of which are accessed using a simple pattern below<br>
replace the "X" with an "H" to get the 8 bit high version (second lowest byte)<br>
replace the "X" with an "L" to get the 8 bit low version (lowest byte)
</p>

<p>
Visual representation of which bits each register is located at using the RAX register as an example:
</p>

<pre>
High                                                                                         Low
┌──────────────────────────────────────────────────────────────────────────────────────────────┐
│                                      RAX (64 bits)                                           │
├───────────────────────────────────────────┬──────────────────────────────────────────────────┤
│                                           │                   EAX (32 bits)                  │
│                                           └────────────────────┬─────────────────────────────┤
│                                                                │        AX (16 bits)         │
│                                                                ├──────────────┬──────────────┤
│                                                                │ AH (8 bits)  │ AL (8 bits)  │
└────────────────────────────────────────────────────────────────┴──────────────┴──────────────┘
│ ┌ ┐ └ ┘ ─ ├ ┬ ┴ ┤ ┼
</pre>



## Register categories (callee, caller)

<p>
Instructions use registers in assembly, but there arent that many registers and we need to make sure that functions dont overwrite each others data in registers.<br>
Thats why we have callee-saved and caller-saved registers<br>
In simple terms:<br>
When one function (caller) wants to call another function (callee) both need to store / restore values if they want to use them.<br>
e.g.<br>
The most commonly used callee-saved register is rbp for the stack frame
</p>

```asm
myFunc:
        push    rbp             ; save previous stack frame
        mov     rbp, rsp        ; set stack frame
        ; ...                   ; function logic
        pop     rbp             ; restore previous stack frame
        ret                     ; return to caller
```

<p>
rbp is used by every function that has a stack frame, and we cant just overwrite rbp every time we call a function without restoring it, that will prevent functions from running after calling another one finishes, so we need to save it (push rbp) and then restore it (pop rbp).<br>
Since rbp is a callee-saved register, it is saved and restored in the callee function (myFunc)
</p>
<p>
rax is caller-saved and commonly used as the return value for functions.<br>
So if the caller wants to prevent this value from being overwritten, it needs to save it before calling a function. 
</p>

```asm
myFunc:
        push    rax             ; save rax
        call    someFunc        ; call a function that might overwrite rax
        pop     rax             ; restore rax
        ; ...                   ; use rax further
```

<p>
Here you can see we are saving rax before calling someFunc because the function can overwrite rax. When the function ends we restore rax so we can use it further.
</p>

<p>
Summary:
Callers dont have to worry about data stored in callee-saved registers when functions are being called since the callee saves and restores them if they are using them.
Callees dont have to worry about data stored in caller-saved registers when they are being called, since it is expected that the caller has already saved the data that can be restored after the function ends.
</p>
<p>
In simple terms:
Callee-saved registers wont be overwritten when functions are called, so they are <b>preserved between function calls</b> and <b>will remain the same after call</b>
Caller-saved registers might be overwritten when functions are called, so they are <b>NOT preserved between function calls</b> and <b>might NOT remain the same after call</b>
</p>
<p>
Why split the responsibility?<br>
If everything were callee-saved, every function would need to push/pop a bunch of registers even if the caller doesn't care about their values = wasted work.<br>
If everything were caller-saved, every caller would need to save all registers before every call = also inefficient.<br>
By splitting, we optimize:<br>
<ul>
<li>Temporary registers: caller-saved (fast, no overhead if you don’t care about them)</li>
<li>Persistent registers: callee-saved (safe across calls)</li>
</ul>
</p>





## Flags
| Flag                   | Meaning                                                                                                                                                   |
| ---------------------- | --------------------------------------------------------------------------------------------------------------------------------------------------------- |
| Z (ZF) (Zero Flag)     | is true if result is 0, indicating equality (e.g. cmp 3, 3 = 3 - 3 = 0)                                                                                   |
| S (SF) (Sign Flag)     | if result is negative, msb is 1 (e.g. cmp 2, 3 = -1 = negative = true)                                                                                    |
| C (CF) (Carry Flag)    | is true when a borrow occurred, meaning that a < b which would result in a negative number but since its UNSIGNED it becomes a borrow (look further into) |
| O (OF) (Overflow Flag) | signed overflow, is only set in SIGNED comparison, is true when the result overflows the signed range (look further into)                                 |
| P (PF) (Parity Flag)   | (less commonly used), set if the least significant byte of the result has an even number of 1 bits                                                        |


## Flag registers
| Bit   | FLAGS (16) | EFLAGS(32) | RFLAGS(64) | Name    | Description                       |
| ----- | ---------- | ---------- | ---------- | ------- | --------------------------------- |
| 0     | Yes        | Yes        | Yes        | CF      | Carry Flag                        |
| 1     | Yes        | Yes        | Yes        | -       | Reserved (always 1 in FLAGS (16)) |
| 2     | Yes        | Yes        | Yes        | PF      | Parity Flag                       |
| 3     | Yes        | Yes        | Yes        | -       | Reserved                          |
| 4     | Yes        | Yes        | Yes        | AF      | Auxiliary Carry Flag              |
| 5     | Yes        | Yes        | Yes        | -       | Reserved                          |
| 6     | Yes        | Yes        | Yes        | ZF      | Zero Flag                         |
| 7     | Yes        | Yes        | Yes        | SF      | Sign Flag                         |
| 8     | Yes        | Yes        | Yes        | TF      | Trap Flag (single-step)           |
| 9     | Yes        | Yes        | Yes        | IF      | Interrupt Enable Flag             |
| 10    | Yes        | Yes        | Yes        | DF      | Direction Flag                    |
| 11    | Yes        | Yes        | Yes        | OF      | Overflow Flag                     |
| 12    | No         | Yes        | Yes        | IOPL[0] | I/O Privilege Level (bit 0)       |
| 13    | No         | Yes        | Yes        | IOPL[1] | I/O Privilege Level (bit 1)       |
| 14    | No         | Yes        | Yes        | NT      | Nested Task                       |
| 15    | No         | Yes        | Yes        | -       | Reserved                          |
| 16    | No         | Yes        | Yes        | RF      | Resume Flag                       |
| 17    | No         | Yes        | Yes        | VM      | Virtual-8086 Mode                 |
| 18    | No         | Yes        | Yes        | AC      | Alignment Check                   |
| 19    | No         | Yes        | Yes        | VIF     | Virtual Interrupt Flag            |
| 20    | No         | Yes        | Yes        | VIP     | Virtual Interrupt Pending         |
| 21    | No         | Yes        | Yes        | ID      | CPUID Enable                      |
| 22-31 | No         | Yes        | Yes        | -       | Reserved                          |
| 32-63 | No         | No         | Yes        | -       | Reserved                          |



## Basic instructions

### cmp
<p>
compares two values and updates flags<br>
it compares them by subtracting the first from the second<br>
e.g. cmp 3, 2      =      3 - 2 = 1
</p>


### mov
<p>
copies data from one register to another<br>
e.g. mov A, B. Copies B into A. Essentially the same as A = B
</p>




## Adding / Subtracting

### add
<p>
adds two values together and upates flags<br>
e.g. add A, B. Adds B onto A. Essentially the same as A += B;
</p>

### sub (same as add, but with subtraction)
<p>
subtracts one value from another and upates flags<br>
e.g. sub A, B. Subtracts B from A. Essentially the same as A -= B;
</p>



## Multiplication

### mul
<p>
multiplies UNSIGNED integers<br>
mul A, B. Multiply A and B. Essentially the same as A *= B;
</p>

### imul
<p>multiplies SIGNED integers</p>
<p>imul A, B. Multiply A and B. Essentially the same as A *= B;
</p>



## Jumps
<p>
jumps go to a target location in the program:
</p>
<ul>
<li>labels (e.g. myLabel)</li>
<li>registers (e.g. the value stored in EAX)</li>
<li>memory location (e.g. [0x1200])</li>
</ul>


### Unconditional Jump
<p>does not check for anything and immediately goes to the desired location</p>

<p>
jmp LOC<br>
e.g. jmp myLabel
</p>



### Conditional Jump
<p>
only jumps based on certain flag values
</p>


#### Equality / zero
| Instruction | Meaning              | Flags |
| ----------- | -------------------- | ----- |
| JE  / JZ    | equal / zero         | Z = 1 |
| JNE / JNZ   | not equal / not zero | Z = 0 |

<br>

#### Carry / unsigned comparisons
| Instruction | Meaning                     | Flags           |
| ----------- | --------------------------- | --------------- |
| JC          | carry                       | C = 1           |
| JNC         | not carry                   | C = 0           |
| JA  / JNBE  | above (unsigned >)          | C = 0 and Z = 0 |
| JAE / JNB   | above (unsigned >) or equal | C = 0           |
| JB  / JNAE  | below (unsigned <)          | C = 1           |
| JBE / JNA   | below (unsigned <) or equal | C = 1 or Z = 1  |

<br>

#### Signed comparisons
| Instruction | Meaning                     | Flags           |
| ----------- | --------------------------- | --------------- |
| JG  / JNLE  | greater (signed >)          | Z = 0 and S = O |
| JGE / JNL   | greater (signed >) or equal | S = O           |
| JL  / JNGE  | less (signed <)             | S != O          |
| JLE / JNG   | less (signed <) or equal    | Z = 1 or S != O |

<br>

#### Sign and overflow
| Instruction | Meaning             | Flags |
| ----------- | ------------------- | ----- |
| JS          | sign (negative)     | S = 1 |
| JNS         | not sign (positive) | S = 0 |
| JO          | sign                | O = 1 |
| JNO         | no sign             | O = 0 |

<br>

#### Parity
| Instruction | Meaning     | Flags |
| ----------- | ----------- | ----- |
| JP  / JPE   | parity even | P = 1 |
| JNP / JPO   | parity odd  | P = 0 |







## Stack / functions

### push
<p>
this does two things<br>
first it decrements (reserves space) the stack pointer (rsp, esp, sp) by the size of the operand<br>
second it stores the operand value at the new address pointed to by the stack pointer<br>
e.g.
</p>

<pre>
push rax

is equivalent to:

sub rsp, 8      ; decrement (reserve space) the stack
mov [rsp], rax  ; save rax on the stack
</pre>

#### Behavior and variants of push
| Name   | Full name        | What gets pushed                                            | Operand size                         |
| ------ | ---------------- | ----------------------------------------------------------- | ------------------------------------ |
| push   | push             | General-purpose register, memory operand or immediate value | Depends on current mode (16, 32, 64) |
| pushf  | push flags       | FLAGS register (16 bit)                                     | 16                                   |
| pushfd | push flags dword | EFLAGS (32 bit)                                             | 32                                   |
| pushfq | push flags qword | RFLAGS (64 bit)                                             | 64                                   |



### pop
<p>
does the opposite of push<br>
removes a value from the stack and stores it in the operand<br>
then it increments (frees space) the stack pointer by the size of the operand<br>
keep in mind, if you store an 8 byte value, then pop into a 1 byte value, the remaining 7 bytes will be unaffected<br>
e.g.
</p>

<pre>
pop rbx

is equivalent to:

mov rbx, [rsp]  ; get top value on the stack
add rsp, 8      ; increment (free space) on the stack
</pre>



### ret
<p>
returns from a function<br>
pops the return address stored in the stack into the rip register<br>
e.g.
</p>

<pre>
ret

is equivalent to:

pop rip
</pre>



### leave
<p>
leave is used to cleanup / restore stack to its original state (before a function was called).<br>
it does two things:
<ul>
<li>Restore old stack pointer</li>
<li>Restore old base pointer</li>
</ul>
all function that use stack space calls (pop rbp) at the end of the function, but only those who reserve more space (sub rsp, N) will need to restore the stack pointer.<br>
In this case, instead of writing (pop rbp), we write (leave) since leave already does (pop rbp) and restores the stack pointer which was modified when reserving more stack space
</p>

<pre>
leave

is equivalent to:

mov     rsp, rbp        ; restore old stack pointer
pop     rbp             ; restore old base pointer
</pre>




### call
<p>
calls a function<br>
pushes the address of the instruction after the call instruction onto the stack<br>
then it jumps to the function<br>
e.g.
</p>

<pre>
call my_function

is equivalent to:

push rip + size_of_call     ; size_of_call is computed by the CPU, it is the offset between the call instruction and the instruction after call, meaning the result of rip + size_of_call is equal to the memory address of the instruction after the call instruction
jmp my_function
</pre>














## Examples






### Squaring a number

C:
```c
int square(int num) {
    return num * num;
}
```
Assembly:
```asm
square:
        push    rbp                     ; save the stack frame from the previous function onto the stack so it can be restored when we return to the previous function
        mov     rbp, rsp                ; move the current stack pointer into rbp for local variable access
        ; sub rsp, 16                   ; if needed, move stack pointer down to reserve space for local variables
        mov     DWORD PTR [rbp-4], edi  ; (function logic) move parameter into local variable
        mov     eax, DWORD PTR [rbp-4]  ; (function logic) move parameter int eax for multiplication
        imul    eax, eax                ; (function logic) multiply eax with eax
        leave                           ; clean up stack frame
        ret                             ; set instruction pointer to instruction after this function was called
```






### For loop printing "Hello, World!"

C:
```c
#include <stdio.h>

void loop() {
    for (int i = 0; i < 10; i++) {
        printf("Hello, world!\n");
    }
}
```
Assembly:
```asm
; loop steps
; condition -> body
;         body -> condition
; else
;         do nothing

main:
        push    rbp                     ; save the stack frame from the previous function onto the stack so it can be restored when we return to the previous function
        mov     rbp, rsp                ; move the current stack pointer into rbp for local variable access
        sub     rsp, 16                 ; move stack pointer down to reserve space for local variables
        mov     DWORD PTR [rbp-4], 0    ; int i = 0
        jmp     .L2                     ; jump to loop condition
.L3:                                    ; loop body
        mov     edi, OFFSET FLAT:.LC0   ; move "Hello, World!\n" to function parameter
        call    puts                    ; call printf with the parameter
        add     DWORD PTR [rbp-4], 1    ; i++
.L2:                                    ; loop condition
        cmp     DWORD PTR [rbp-4], 9    ; i < 10
        jle     .L3                     ; jump to loop body if number is less than (signed) or equal to 9
        nop                             ; alignment
        nop                             ; alignment
        leave                           ; clean up stack frame
        ret                             ; set instruction pointer to instruction after this function was called
```


Alternative assembly code (less preferred):
```asm
; loop steps
; condition -> do nothing
;         do nothing
; else
;         body -> condition

main:
        push    rbp                     ; save the stack frame from the previous function onto the stack so it can be restored when we return to the previous function
        mov     rbp, rsp                ; move the current stack pointer into rbp for local variable access
        sub     rsp, 16                 ; move stack pointer down to reserve space for local variables
        mov     DWORD PTR [rbp-4], 0    ; int i = 0
.L2                                     ; compare and loop body
        cmp     DWORD PTR [rbp-4], 10   ; i != 10
        je     .L3                      ; jump to loop body if number is less than (signed) or equal to 9
        mov     edi, OFFSET FLAT:.LC0   ; move "Hello, World!\n" to function parameter
        call    puts                    ; call printf with the parameter
        add     DWORD PTR [rbp-4], 1    ; i++
        jmp     .L2                     ; continue loop
.L3:                                    ; end of loop
        nop                             ; alignment
        nop                             ; alignment
        leave                           ; clean up stack frame
        ret                             ; set instruction pointer to instruction after this function was called
```














## C basics examples




### Simple math

C:
```c
int main() {
    int a = 5;
    int b = 3;
    int c = a * b + 2;
    return c;
}
```
Assembly:
```asm
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     DWORD PTR [rbp-4], 5            ; int a = 5;
        mov     DWORD PTR [rbp-8], 3            ; int b = 3;
        mov     eax, DWORD PTR [rbp-4]          ; eax = a
        imul    eax, DWORD PTR [rbp-8]          ; multiply eax (a) with b
        add     eax, 2                          ; add 2 to eax
        mov     DWORD PTR [rbp-12], eax         ; int c = eax (a * b + 2)
        mov     eax, DWORD PTR [rbp-12]         ; set eax to c, which is the return value
        pop     rbp                             ; restore previous stack frame
        ret                                     ; return to caller
```











### Conditional

C:
```c
int main() {
    int x = 7;
    if (x > 5)
        x = x - 1;
    else
        x = x + 1;
    return x;
}
```
Assembly:
```asm
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     DWORD PTR [rbp-4], 7            ; int x = 7
        cmp     DWORD PTR [rbp-4], 5            ; compare x with 5
        jle     .L2                             ; jump to "else" if x is less than or equal to 5
        sub     DWORD PTR [rbp-4], 1            ; "if" body: subtract 1 from x
        jmp     .L3                             ; jump to end of if statement
.L2:                                            ; "else"
        add     DWORD PTR [rbp-4], 1            ; add 1 to x
.L3:                                            ; end of if statement
        mov     eax, DWORD PTR [rbp-4]          ; set eax to x, which is the return value
        pop     rbp                             ; restore previous stack frame
        ret                                     ; return to caller
```











### For loop

C:
```c
int main() {
    int sum = 0;
    for (int i = 0; i < 5; i++)
        sum += i;
    return sum;
}
```
Assembly:
```asm
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     DWORD PTR [rbp-4], 0            ; int sum = 0
        mov     DWORD PTR [rbp-8], 0            ; int i = 0
        jmp     .L2                             ; jump to loop condition
.L3:                                            ; loop body
        mov     eax, DWORD PTR [rbp-8]          ; move i into eax (most assembly instructions dont allow 2 memory locations, but do allow for one register and one memory location)
        add     DWORD PTR [rbp-4], eax          ; add eax (i) to sum (sum += i)
        add     DWORD PTR [rbp-8], 1            ; i++
.L2:                                            ; loop condition
        cmp     DWORD PTR [rbp-8], 4            ; compare eax (i) to 4
        jle     .L3                             ; jump if less than or equal to 4 (i < 5 same as i <= 4)
        mov     eax, DWORD PTR [rbp-4]          ; move sum to eax, which is the return value
        pop     rbp                             ; restore previous stack frame
        ret                                     ; return to caller
```











### While loop and break

C:
```c
int main() {
    int i = 0;
    int total = 0;
    while (1) {
        total += i;
        if (i >= 3)
            break;
        i++;
    }
    return total;
}
```
Assembly:
```asm
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     DWORD PTR [rbp-4], 0            ; int i = 0
        mov     DWORD PTR [rbp-8], 0            ; int total = 0
.L4:                                            ; while body
        mov     eax, DWORD PTR [rbp-4]          ; move i into eax
        add     DWORD PTR [rbp-8], eax          ; add eax (i) onto total (total += i)
        cmp     DWORD PTR [rbp-4], 2            ; compare i to 2
        jg      .L7                             ; jump if i > 2 (i >= 3), this is the break statement and the jump jumps to the end of the loop
        add     DWORD PTR [rbp-4], 1            ; i++
        jmp     .L4                             ; jump to while body
.L7:                                            ; end of while
        nop                                     ; nop for alignment
        mov     eax, DWORD PTR [rbp-8]          ; move total to eax (return value)
        pop     rbp                             ; restore previous stack frame
        ret                                     ; return to caller
```











### Function call

C:
```c
int add(int a, int b) {
    return a + b;
}

int main() {
    return add(2, 3);
}
```
Assembly:
```asm
add:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     DWORD PTR [rbp-4], edi          ; move (int a) parameter into local variable space
        mov     DWORD PTR [rbp-8], esi          ; move (int b) parameter into local variable space
        mov     edx, DWORD PTR [rbp-4]          ; move (a) into edx
        mov     eax, DWORD PTR [rbp-8]          ; move (b) into eax
        add     eax, edx                        ; add eax (a) and edx (b) and store the result in eax (eax is the return value)
        pop     rbp                             ; restore previous stack frame
        ret                                     ; return to caller
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     esi, 3                          ; move 3 into parameter (int b)
        mov     edi, 2                          ; move 2 into parameter (int a)
        call    add                             ; call add
        pop     rbp                             ; restore previous stack frame
        ret                                     ; return to caller
```











### Multiple functions

C:
```c
int square(int x) {
    return x * x;
}

int cube(int x) {
    return x * square(x);
}

int main() {
    return cube(3);
}
```
Assembly:
```asm
square:                                         ; square an integer
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     DWORD PTR [rbp-4], edi          ; move 1st parameter (int x) into local variable space
        mov     eax, DWORD PTR [rbp-4]          ; move (int x) into eax
        imul    eax, eax                        ; multiply eax with eax (x * x)
        pop     rbp                             ; restore previous stack frame
        ret                                     ; return to caller
cube:                                           ; cube an integer
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        sub     rsp, 8                          ; reserve 8 bytes of stack space (for alignment)
        mov     DWORD PTR [rbp-4], edi          ; move 1st parameter (int x) into local variable space
        mov     eax, DWORD PTR [rbp-4]          ; move (int x) into eax
        mov     edi, eax                        ; move eax (int x) into edi (1st parameter)
        call    square                          ; call square
        imul    eax, DWORD PTR [rbp-4]          ; multiply eax (return value of square) with (int x)
        leave                                   ; restore previous stack ("leave" instead of "pop rbp" because of "sub rsp, 8", see "leave" for more)
        ret                                     ; return to caller
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     edi, 3                          ; move 3 into edi (1st function parameter)
        call    cube                            ; call cube
        pop     rbp                             ; restore previous stack
        ret                                     ; return to caller
```











### Arrays and pointers

C:
```c
int main() {
    int nums[3] = {1, 2, 3};
    int sum = nums[0] + nums[1] + nums[2];
    return sum;
}
```
Assembly:
```asm
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     DWORD PTR [rbp-16], 1           ; nums[0] = 1
        mov     DWORD PTR [rbp-12], 2           ; nums[1] = 2
        mov     DWORD PTR [rbp-8], 3            ; nums[2] = 3
        mov     edx, DWORD PTR [rbp-16]         ; move nums[0] into edx
        mov     eax, DWORD PTR [rbp-12]         ; move nums[1] into eax
        add     edx, eax                        ; add eax onto edx
        mov     eax, DWORD PTR [rbp-8]          ; move nums[3] into eax
        add     eax, edx                        ; add edx onto eax
        mov     DWORD PTR [rbp-4], eax          ; move the result into (int sum)
        mov     eax, DWORD PTR [rbp-4]          ; move (int sum) into eax (return value)
        pop     rbp                             ; restore previous stack frame
        ret                                     ; return to caller
```











### Pointer arithmetic

C:
```c
int main() {
    int x = 10;
    int *p = &x;
    *p = *p + 5;
    return x;
}
```
Assembly:
```asm
main:
                                                ; stack
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame

                                                ; populate (x)
        mov     DWORD PTR [rbp-12], 10          ; int x = 10
        
                                                ; populate (p)
        lea     rax, [rbp-12]                   ; place the memory address of (x) into rax
        mov     QWORD PTR [rbp-8], rax          ; move the value in rax (&x) into (p)

                                                ; perform (*p = *p + 5)
        mov     rax, QWORD PTR [rbp-8]          ; move (p) into rax
        mov     eax, DWORD PTR [rax]            ; dereference (*p) into eax (read memory stored at memory address (*p))
        lea     edx, [rax+5]                    ; use "lea" to do math (move rax+5 into edx)
        mov     rax, QWORD PTR [rbp-8]          ; move (p) into rax
        mov     DWORD PTR [rax], edx            ; move edx into the dereference (*p) (update memory stored at memory address (*p))

                                                ; return x
        mov     eax, DWORD PTR [rbp-12]         ; move x into eax (return value)

                                                ; stack
        pop     rbp                             ; restore previous stack frame
        ret                                     ; return to caller
```











### Recursion

C:
```c
int fact(int n) {
    if (n <= 1) return 1;
    return n * fact(n - 1);
}

int main() {
    return fact(4);
}
```
Assembly:
```asm
fact:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        sub     rsp, 16                         ; allocate more space / alignment
        mov     DWORD PTR [rbp-4], edi          ; move edi into local variable space (int n)
        cmp     DWORD PTR [rbp-4], 1            ; compare (int n) with 1
        jg      .L2                             ; jump if greater than 1
        mov     eax, 1                          ; move 1 into eax (return value)
        jmp     .L3                             ; jump to end of function
.L2:                                            ; if not base case
        mov     eax, DWORD PTR [rbp-4]          ; move n into eax
        sub     eax, 1                          ; subtract 1 from eax (n - 1)
        mov     edi, eax                        ; move eax into 1st function parameter
        call    fact                            ; call fact
        imul    eax, DWORD PTR [rbp-4]          ; multiply the result of (fact(n - 1)) with (n)
.L3:                                            ; base case
        leave                                   ; restore previous stack
        ret                                     ; return to caller
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     edi, 4                          ; move 4 into 1st function parameter
        call    fact                            ; call fact
        pop     rbp                             ; restore previous stack frame
        ret                                     ; return to caller
```











### Structs

C:
```c
struct Point {
    int x;
    int y;
};

int main() {
    struct Point p = {3, 4};
    return p.x * p.y;
}
```
Assembly:
```asm
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     DWORD PTR [rbp-8], 3            ; move 3 into x
        mov     DWORD PTR [rbp-4], 4            ; move 4 into y
        mov     edx, DWORD PTR [rbp-8]          ; move x into edx
        mov     eax, DWORD PTR [rbp-4]          ; move y into eax
        imul    eax, edx                        ; multiply eax with edx (eax is return value)
        pop     rbp                             ; restore previous stack frame
        ret                                     ; return to caller
```











### Static variables

C:
```c
int counter() {
    static int x = 0;
    x++;
    return x;
}

int main() {
    return counter() + counter();
}
```
Assembly:
```asm
counter:
        push    rbp                             ; save previous stack pointer
        mov     rbp, rsp                        ; set stack pointer
        mov     eax, DWORD PTR x.0[rip]         ; "x.0" is literally just a name / symbol, just like ".L2" for jump labels
        add     eax, 1                          ; add 1 to eax
        mov     DWORD PTR x.0[rip], eax         ; move eax back into (static int x)
        mov     eax, DWORD PTR x.0[rip]         ; move (static int x) into eax (return value)
        pop     rbp                             ; restore previous stack
        ret                                     ; return to caller
main:
        push    rbp                             ; save previous stack pointer
        mov     rbp, rsp                        ; set stack pointer
        push    rbx                             ; save rbx (callee saved)
        call    counter                         ; call counter
        mov     ebx, eax                        ; move eax into ebx
        call    counter                         ; call counter
        add     eax, ebx                        ; add ebx onto eax
        mov     rbx, QWORD PTR [rbp-8]          ; we can (pop rbx) but does not have any functional difference since (leave) will restore rsp to its previous state. This way of restoring rbx uses more memory ("pop rbx" is one byte while "mov rbx, QWORD PTR [rbp-8]" is 4-7 bytes). While "pop rbx" uses 2 uops, "mov rbx, QWORD PTR [rbp-8]" only uses 1
        leave                                   ; restore previous stack
        ret                                     ; return to caller
```











### Inline function vs macro vs expression

C:
```c
inline int square_inline(int x) { return x * x; }
#define SQUARE_MACRO(x) ((x) * (x))

int main() {
    int a = square_inline(5);
    int b = SQUARE_MACRO(5);
    int c = 5 * 5;
    return a + b;
}
```
Assembly:
```asm
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        sub     rsp, 16                         ; alignment
        mov     edi, 5                          ; move 5 into edi (1st function parameter)
        call    square_inline                   ; call non existent function (in -O0, inline functions may not be defined)
        mov     DWORD PTR [rbp-4], eax          ; move return value from "square_inline" into stack variable (int a)
        mov     DWORD PTR [rbp-8], 25           ; move 25 into into stack variable (int b). As you can see it has been computed at compile time
        mov     DWORD PTR [rbp-12], 25          ; move 25 into into stack variable (int c). As you can see it has been computed at compile time
        mov     edx, DWORD PTR [rbp-4]          ; move stack variable into edx (int a)
        mov     eax, DWORD PTR [rbp-8]          ; move stack variable into eax (int b)
        add     edx, eax                        ; add eax onto edx
        mov     eax, DWORD PTR [rbp-12]         ; move stack variable into eax (int c)
        add     eax, edx                        ; add edx onto eax
        leave                                   ; restore previous stack
        ret                                     ; return to caller
```











### Bitwise operations

C:
```c
int main() {
    int x = 6;
    x = x << 1;
    x = x << 2;
    x = x | 3;
    x = x & 7;
    x = x ^ 4;
    x = ~5;
    return x;
}
```
Assembly:
```asm
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     DWORD PTR [rbp-4], 6            ; move 6 into stack variable (int x)
        sal     DWORD PTR [rbp-4]               ; bit shift left (<<) by 1 (1 is default if second parameter is omitted)
        sal     DWORD PTR [rbp-4], 2            ; bit shift left (<<) by 2
        or      DWORD PTR [rbp-4], 3            ; bit or (|) with 3
        and     DWORD PTR [rbp-4], 7            ; bit and (&) with 7
        xor     DWORD PTR [rbp-4], 4            ; bit xor (^) with 4
        mov     DWORD PTR [rbp-4], -6           ; move (~5) into (int x). See "Two's complement" for more
        mov     eax, DWORD PTR [rbp-4]          ; move (int x) into eax (return value)
        pop     rbp                             ; restore previous stack
        ret                                     ; return to caller
```







### Switch

C:
```c
int main() {
    int a = 5;
    int b;

    switch (a) {
        case 0:
            b = 2;
            break;
        case 1:
            b = 3;
            break;
        case 2:
            b = 4;
            break;
        case 3:
            b = 5;
            break;
        case 4:
            b = 6;
            break;
        case 5:
            b = 7;
            break;
        default:
            break;
    }
}
```

Assembly:
```asm
main:
        push    rbp                             ; save previous stack frame
        mov     rbp, rsp                        ; set stack frame
        mov     DWORD PTR [rbp-4], 5            ; int a = 5
        cmp     DWORD PTR [rbp-4], 5            ; compare a to 5
        je      .L2                             ; jump if equal
        cmp     DWORD PTR [rbp-4], 5            ; compare a to 5
        jg      .L11                            ; jump if greater
        cmp     DWORD PTR [rbp-4], 4            ; compare a to 4
        je      .L4                             ; jump if equal
        cmp     DWORD PTR [rbp-4], 4            ; compare a to 4
        jg      .L11                            ; jump if greater
        cmp     DWORD PTR [rbp-4], 3            ; compare a to 3
        je      .L5                             ; jump if equal
        cmp     DWORD PTR [rbp-4], 3            ; compare a to 3
        jg      .L11                            ; jump if greater
        cmp     DWORD PTR [rbp-4], 2            ; compare a to 2
        je      .L6                             ; jump if equal
        cmp     DWORD PTR [rbp-4], 2            ; compare a to 2
        jg      .L11                            ; jump if greater
        cmp     DWORD PTR [rbp-4], 0            ; compare a to 0
        je      .L7                             ; jump if equal
        cmp     DWORD PTR [rbp-4], 1            ; compare a to 1
        je      .L8                             ; jump if equal
        jmp     .L11                            ; jump to default case
.L7:
        mov     DWORD PTR [rbp-8], 2            ; b = 2
        jmp     .L9                             ; break
.L8:
        mov     DWORD PTR [rbp-8], 3            ; b = 3
        jmp     .L9                             ; break
.L6:
        mov     DWORD PTR [rbp-8], 4            ; b = 4
        jmp     .L9                             ; break
.L5:
        mov     DWORD PTR [rbp-8], 5            ; b = 5
        jmp     .L9                             ; break
.L4:
        mov     DWORD PTR [rbp-8], 6            ; b = 6
        jmp     .L9                             ; break
.L2:
        mov     DWORD PTR [rbp-8], 7            ; b = 7
        jmp     .L9                             ; break
.L11:
        nop                                     ; default case
.L9:
        mov     eax, 0                          ; move 0 into eax (return value)
        pop     rbp                             ; restore previous stack
        ret                                     ; return to caller
```