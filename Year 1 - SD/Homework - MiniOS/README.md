Homework - MiniOS

The purpose of this homework was simulating the use of an operating system by
managing its memory and processes. The MiniOS stores the processes in 3MiB.
Because the memory can be fragmentated, it automatically calls the defragmentation
function if a new process can't be added in any empty areas of memory so that all
existing processes are rearranged in the memory by their PIDs and all empty
memory is located at the end in one continuous area.

The code ilustrates the use of generic singly-linked lists, queues, stacks and
dynamic memory allocation.