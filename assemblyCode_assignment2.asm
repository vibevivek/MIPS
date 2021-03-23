.data
  stack: .space 212
  userInput: .space 53
  askLength: .asciiz "Enter the length of input string: "
  askString: .asciiz "Enter postfix expression: \n"
  invalidPostfixExpression: .asciiz "Given postfix expression is invalid \n"
  programClosed: .asciiz "Program Closed \n"
  invalidCharacters: .asciiz "Invalid characters found in postfix expression \n"
  naturalNum: .asciiz "Length of string should be a natural number \n"
  lengthOverflow: .asciiz "Sorry!! Input length greater than 51 can't be evluated!! \n"
  smallInput: .asciiz "Input string length less than mentioned \n"
  newLine: .asciiz "\n"
  result_msg: .asciiz "Postfix expression evaluates to: "
  overflow_msg: .asciiz "Calculation overflow. \n"


.text
  main:
    # Dispaly msg to ask length of input string
    li $v0, 4
    la $a0, askLength
    syscall

    # Read length of input string and store in $t0 (n)
    li $v0, 5
    syscall
    move $t0, $v0

    # n<=0
    blez $t0, emptyStringInput

    addi $t1, $zero, 51 # $t1 stores max input string length allowed
    # n>20
    blt $t1, $t0, TooLargeInput

    li $v0, 4
    la $a0, askString
    li $a1, 52
    syscall

    li $v0, 8
    la $a0, userInput
    syscall

    la $s0, userInput
    li $t1, 0 # $t1->i (to iterate over the postfix expression)
    li $t3, 0
    j parse

    # exit program
    j exit
  

  parse:
    beq $t1, $t0, result # i==n
    lb $t2, ($s0) # read character in $t2
    beq $t2, 10, unexpectedEnding # newLine character (input string length less than as expected)

    beq $t2, 43, addition # ascii for +
    beq $t2, 45, subtraction # ascii for -
    beq $t2, 42, multiplication # ascii for *

    blt $t2, 48, invalidChar	# throw exception for ascii value below '0'
	  bgt	$t2, 57, invalidChar  # throw exception for ascii value above '9'

    sub $t2, $t2, 48			# convert from character to number

    move $a0, $t2

    jal stack_push 

    addi $s0, $s0, 1
    addi $t1, $t1, 1
    j parse


  stack_push:
    sw $a0, stack($t3)
    addi $t3, $t3, 4
    jr $ra
  
  stack_pop:
    beq $t3, $zero, invalidFormat

    addi $t3, $t3, -4
    lw $v0, stack($t3)
    jr $ra


  addition:
    jal stack_pop
    move $t4, $v0

    jal stack_pop
    move $t5, $v0

    addu $t6, $t5, $t4

    # overflow check
    xor $s7, $t4, $t6
    xor $s6, $t5, $t6
    and $s7, $s6, $s7
    bltz $s7, overflow


    move $a0, $t6
    jal stack_push

    addi $s0, $s0, 1
    addi $t1, $t1, 1
    j parse

  subtraction:
    jal stack_pop
    move $t4, $v0

    jal stack_pop
    move $t5, $v0

    subu $t6, $t5, $t4

    # overflow detection
    xor $s7, $t4, $t5
    xor $s6, $t5, $t6
    and $s7, $s6, $s7
    bltz $s7, overflow


    move $a0, $t6
    jal stack_push

    addi $s0, $s0, 1
    addi $t1, $t1, 1
    j parse

  multiplication:
    jal stack_pop
    move $t4, $v0

    jal stack_pop
    move $t5, $v0

    # overflow check
    multu $t4, $t5
    mflo $s7
    mfhi $s6
    sra $s7, $s7, 31
    bne $s6, $s7, overflow
    
    
    mflo $t6
    move $a0, $t6
    jal stack_push

    addi $s0, $s0, 1
    addi $t1, $t1, 1
    j parse

  
  result: 
    jal stack_pop
    move $t5, $v0

    bnez $t3, invalidFormat

    li $v0, 4
    la $a0, result_msg
    syscall

    li $v0, 1
    move $a0, $t5
    syscall

    li $v0, 4
    la $a0, newLine
    syscall

    j exit


  exit:
    li $v0, 4
    la $a0, programClosed
    syscall

    li $v0, 10
    syscall  

  emptyStringInput:
    li $v0, 4
    la $a0, naturalNum
    syscall
    
    j exit

  TooLargeInput:
    li $v0, 4
    la $a0, lengthOverflow
    syscall
    
    j exit

  invalidFormat:
    li $v0, 4
    la $a0, invalidPostfixExpression
    syscall
    
    j exit

  invalidChar:
    li $v0, 4
    la $a0, invalidCharacters
    syscall
    
    j exit

  unexpectedEnding:
    li $v0, 4
    la $a0, smallInput
    syscall
    
    j exit

  overflow:
    li $v0, 4
    la $a0, overflow_msg
    syscall

    j exit
