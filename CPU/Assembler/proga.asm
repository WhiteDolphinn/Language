push 35
pop bx

push 3.000000
pop [0]
push 0.000000
pop [1]
jmp :across_func_0

:func_0
pop [bx]
push bx
push 1
add
pop bx
push 1.000000
pop [3]
push 0.000000
pop [4]
:if_0
push [2]
push 1.000000
jne :end_if_0
push 1.000000
pop [3]
:end_if_0
:if_1
push [2]
push 1.000000
ja :end_if_1
push [2]
push [2]
push 1.000000
sub
call :func_0
mul
pop [3]
:end_if_1
push [3]
push bx
push 1
sub
pop bx
push [bx]
ret

:across_func_0
push [0]
call :func_0
pop [1]
push [1]
out
hlt
