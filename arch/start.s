.global _start
_start:
	//system mode
 	mov r0, #0xDF
 	msr cpsr_c, r0

 	//set stack and jump to c
	ldr sp, =0x8000

	bl main
	b .


.global enable_cache
enable_cache:
	push {r0, r1}
	//enable L2 cache
	mrc p15,0,r0,c1,c0,0
	ldr r1, =0x1804
    orr r0, r0, r1
    mcr p15,0,r0,c1,c0,0
	pop {r0, r1}
	bx lr
