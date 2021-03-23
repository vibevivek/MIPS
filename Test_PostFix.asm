////////////////////////TEST CASES//////////////////////////////
=============================
//test case 1 (all operator)
addi $t0, $zero, 100
addi $t1, $zero, 50
add $t3, $t0, $t1
sub $t4, $t0, $t1
mul $t5, $t0, $t1
slt $t6, $t0, $t1

==============================
//test case 2 (lw, sw)
lw $t2, 124
sw $t3, 256
lw $t4, 0($sp)
sw $t5, 0($sp)

===============================
//test case 3 (infinite loop)
addi $t0, $zero, 123
addi $t1, $zero, 321
label_1:
	j label_2
label_2:
	j label_1

===============================
//test case 4 (general)
addi $t0, $zero, 52
addi $t1, $zero, 25
addi $s0, $zero, 25

add $t2, $t0, $t1
sub $t3, $t0, $t1
mul $t4, $t0, $t1
bne $s0, $t1, label_bne
beq $t0, $t4, label_beq
slt $s1, $s0, $t0
slt $v1, $t4, $t2
j label_j

label_beq:
	mul $s1, $s1, $s1
label_bne:
	add $s3, $t4, $t2
	j label_j
test:
	addi $t9, $zero, 100
	j end
label_j:
	sub $t8, $t1, $t0
	j test
end:


=================================
//test case 5(empty)

=================================
//test case 6 ()
sw $t0, 124
lw $t1, 56
add $s1, $t0, $t1
sub $s2, $s1, $t1
mul $s3, $s2, $s1
beq $s2, $t0, label_1
bne $s3, $s2, label_2

label_2:
	add $s1, $s1, $s1
	lw $t4, 0($sp)
		
label_1:
	sub $s2, $s2, $s2
	sw $t4, 0($sp)

=================================
//test case 7(extra arguments)
addi $t0, $zero, 21, $t3
================================
//test case 8 (less arguments)
addi $t0, $zero