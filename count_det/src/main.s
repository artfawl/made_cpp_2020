.text

.global count_det //det = 1*5*9 + 2*6*7 + 3*4*8 - (3*5*7 + 2*4*9 + 1*6*8) цифры - индексы массива

count_det:
    mov r3, #0  //в r0 массив, в r3 считаем det , r1,r2 - вспомогательные регистры

    ldr r1, [r0, #0]
    ldr r2, [r0, #16]
    mul r1, r2
    ldr r2, [r0, #32]
    mul r1, r2
    add r3, r1

    ldr r1, [r0, #4]
    ldr r2, [r0, #20]
    mul r1, r2
    ldr r2, [r0, #24]
    mul r1, r2
    add r3, r1

    ldr r1, [r0, #8]
    ldr r2, [r0, #12]
    mul r1, r2
    ldr r2, [r0, #28]
    mul r1, r2
    add r3, r1

    ldr r1, [r0, #8]
    ldr r2, [r0, #16]
    mul r1, r2
    ldr r2, [r0, #24]
    mul r1, r2
    sub r3, r1

    ldr r1, [r0, #4]
    ldr r2, [r0, #12]
    mul r1, r2
    ldr r2, [r0, #32]
    mul r1, r2
    sub r3, r1

    ldr r1, [r0, #0]
    ldr r2, [r0, #20]
    mul r1, r2
    ldr r2, [r0, #28]
    mul r1, r2
    sub r3, r1

    

    mov r0, r3 
    bx lr
