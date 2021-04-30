**How to Use This Simulator
1.	Put the assembly instructions you want to run into MIPSInstruction.txt
    The instructions should be like this:
        LUI $t1 0x1234
        ORI $t1 $t1 0x5678 // t1=0x12345678
        LUI $t2 0x0000
        ORI $t2 $t2 0x0000 //t2=0x00000000
        SW $t1 0x0000 $t2   //Store 12345678 in address 00000000
        ANDI $t1 $t1 0x0000 //t1=0000
        LUI $t1 0xABCD 	    //t1=ABCD0000
        ORI $t1 $t1 0xEF12  //t1=ABCDEF12
        SW $t1 0x0004 $t2   //Store ABCDEF12 in address 00000004
        ANDI $t1 $t1 0x0000  

2.	Run the main.exe.
3.	You can get the converted binary machine in BinaryCode.txt, like this:
        00111100000010010001001000110100
        00110101001010010101011001111000
        00111100000010100000000000000000
        00110101010010100000000000000000
        10101101010010010000000000000000
        00110001001010010000000000000000
        00111100000010011010101111001101
        00110101001010011110111100010010
        10101101010010010000000000000100
        00110001001010010000000000000000

4.	Commands:
        init:		initialize the simulator
        exit:		exit the simulator
		uup:	    output the utilization of unpipelined mode
		upipe:	    output the utilization of pipeline mode
		time: 	    output the total time
		rup:		run the simulator in unpipelined mode
		rpins:	    run the simulator in pipeline instruction mode
		rpclk:	    run the simulator in pipeline cycle mode
		imem:	    output the IMem contents
		dmem: 	    output the DMem contents
		reg:		output the register contents
		latch:	    output the latch values
		ins ls:	    output the input assembly instructions and the PC location
		tgpipe:	    output the time graph of instruction and pipeline running process 	

