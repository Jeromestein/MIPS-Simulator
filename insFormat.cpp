/*
    Syntax: ArithLog  
    Encoding: Register  
    f $d, $s, $t
    
            31..26  25..21  20..16  15..11  10..6   5..0
            op      rs      rt      rd      shamt   func
    add :   000000  rs      rt      rd      00000   100000
    sub :   000000  rs      rt      rd      00000   100010 
    and :   000000  rs      rt      rd      00000   100100
    or  :   000000  rs      rt      rd      00000   100101
    mul(t): 000000  rs      rt      rd      00000   011000

    // mul(mult)
    // mul $a,$b,$c, 
    // meaning that we multiply the contents of $b and $c, the least significant
    // 32 bits of results are placed in register $a and the most significant 32-bits of the result will be
    // stored in register $(a+1).

    */
/*
    Syntax: Shift  
    Encoding: Register  
    f $d, $t, a

            31..26  25..21  20..16  15..11  10..6   5..0
            op      rs      rt      rd      shamt   func
    sll :   000000  00000   rt      rd      shamt   000000
    srl :   000000  00000   rt      rd      shamt   000010 
    */
/*
    Syntax: ArithLogI  
    Encoding: Immediate  
    o $t, $s, i

            31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    addi:   001000  rs      rt      immediate
    andi:   001100  rs      rt      immediate
    ori :   001101  rs      rt      immediate
    slti:   001010  rs      rt      immediate
    sltiu:  001011  rs      rt      immediate
    */
/*
    Syntax: LoadStore  
    Encoding: Immediate  
    op $t, i, $s
    (lui $t, i)

            31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    lw  :   100011  rs      rt      immediate
    sw  :   101011  rs      rt      immediate
    lui :   001111  00000   rt      immediate    
    */
/*
    Syntax: Branch  
    Encoding: Immediate  
    o $s, $t, label

            31..26  25..21  20..16  15..0
            op      rs      rt      immediate
    beq :   000100  rs      rt      immediate
    */