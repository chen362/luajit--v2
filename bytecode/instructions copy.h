static constexpr uint16_t BC_OP_JMP_BIAS = 0x8000;

enum BC_OP {
	BC_OP_ISLT = 0, // if A<VAR> < D<VAR> then JMP
	BC_OP_ISGE = 1, // if not (A<VAR> < D<VAR>) then JMP
	BC_OP_ISLE = 2, // if A<VAR> <= D<VAR> then JMP
	BC_OP_ISGT = 3, // if not (A<VAR> <= D<VAR>) then JMP
	BC_OP_ISEQV = 4, // if A<VAR> == D<VAR> then JMP
	BC_OP_ISNEV = 5, // if A<VAR> ~= D<VAR> then JMP
	BC_OP_ISEQS = 6, // if A<VAR> == D<STR> then JMP
	BC_OP_ISNES = 7, // if A<VAR> ~= D<STR> then JMP
	BC_OP_ISEQN = 8, // if A<VAR> == D<NUM> then JMP
	BC_OP_ISNEN = 9, // if A<VAR> ~= D<NUM> then JMP
	BC_OP_ISEQP = 10, // if A<VAR> == D<PRI> then JMP
	BC_OP_ISNEP = 11, // if A<VAR> ~= D<PRI> then JMP
	BC_OP_ISFC = 12, // if not D<VAR> then A<DST> = D and JMP (本体，12-20都映射到这里)
	BC_OP_ISFC_ALT1 = 13, // BC_ISFC 别名
	BC_OP_ISFC_ALT2 = 14, // BC_ISFC 别名
	BC_OP_ISFC_ALT3 = 15, // BC_ISFC 别名
	BC_OP_ISFC_ALT4 = 16, // BC_ISFC 别名
	BC_OP_ISFC_ALT5 = 17, // BC_ISFC 别名
	BC_OP_ISFC_ALT6 = 18, // BC_ISFC 别名
	BC_OP_ISFC_ALT7 = 19, // BC_ISFC 别名
	BC_OP_ISFC_ALT8 = 20, // BC_ISFC 别名
	BC_OP_ISTC = 21, // if D<VAR> then A<DST> = D and JMP
	BC_OP_ISF = 22, // if not D<VAR> then JMP
	BC_OP_IST = 23, // if D<VAR> then JMP
	BC_OP_ISTYPE = 0xFF10, // unsupported
	BC_OP_ISNUM = 0xFF11, // unsupported
	BC_OP_MOV = 27, // A<DST> = D<VAR>
	BC_OP_NOT = 26, // A<DST> = not D<VAR>
	BC_OP_UNM = 29, // A<DST> = -D<VAR>
	BC_OP_LEN = 28, // A<DST> = #D<VAR>
	BC_OP_ADDVN = 34, // A<DST> = B<VAR> + C<NUM>
	BC_OP_SUBVN = 35, // A<DST> = B<VAR> - C<NUM>
	BC_OP_MULVN = 36, // A<DST> = B<VAR> * C<NUM>
	BC_OP_DIVVN = 37, // A<DST> = B<VAR> / C<NUM>
	BC_OP_MODVN = 38, // A<DST> = B<VAR> % C<NUM>
	BC_OP_ADDNV = 39, // A<DST> = C<NUM> + B<VAR>
	BC_OP_SUBNV = 40, // A<DST> = C<NUM> - B<VAR>
	BC_OP_MULNV = 41, // A<DST> = C<NUM> * B<VAR>
	BC_OP_DIVNV = 42, // A<DST> = C<NUM> / B<VAR>
	BC_OP_MODNV = 43, // A<DST> = C<NUM> % B<VAR>
	BC_OP_ADDVV = 44, // A<DST> = B<VAR> + C<VAR>
	BC_OP_SUBVV = 45, // A<DST> = B<VAR> - C<VAR>
	BC_OP_MULVV = 46, // A<DST> = B<VAR> * C<VAR>
	BC_OP_DIVVV = 47, // A<DST> = B<VAR> / C<VAR>
	BC_OP_MODVV = 48, // A<DST> = B<VAR> % C<VAR>
	BC_OP_POW = 49, // A<DST> = B<VAR> ^ C<VAR>
	BC_OP_CAT = 50, // A<DST> = B<RBASE> .. B++ -> C<RBASE>
	BC_OP_KSTR = 62, // A<DST> = D<STR>
	BC_OP_KCDATA = 63, // A<DST> = D<CDATA>
	BC_OP_KSHORT = 64, // A<DST> = D<LITS>
	BC_OP_KNUM = 65, // A<DST> = D<NUM>
	BC_OP_KPRI = 66, // A<DST> = D<PRI>
	BC_OP_KNIL = 67, // A<BASE>, A++ -> D<BASE> = nil
	BC_OP_UGET = 55, // A<DST> = D<UV>
	BC_OP_USETV = 56, // A<UV> = D<VAR>
	BC_OP_USETS = 57, // A<UV> = D<STR>
	BC_OP_USETN = 58, // A<UV> = D<NUM>
	BC_OP_USETP = 59, // A<UV> = D<PRI>
	BC_OP_UCLO = 60, // upvalue close for A<RBASE>, A++ -> framesize; goto D<JUMP>
	BC_OP_FNEW = 61, // A<DST> = D<FUNC>
	BC_OP_TNEW = 68, // A<DST> = {}
	BC_OP_TDUP = 69, // A<DST> = D<TAB>
	BC_OP_GGET = 70, // A<DST> = _G.D<STR>
	BC_OP_GSET = 71, // _G.D<STR> = A<VAR>
	BC_OP_TGETV = 72, // A<DST> = B<VAR>[C<VAR>]
	BC_OP_TGETS = 73, // A<DST> = B<VAR>[C<STR>]
	BC_OP_TGETB = 74, // A<DST> = B<VAR>[C<LIT>]
	BC_OP_TGETR = 75, // unsupported
	BC_OP_TSETV = 76, // B<VAR>[C<VAR>] = A<VAR>
	BC_OP_TSETS = 77, // B<VAR>[C<STR>] = A<VAR>
	BC_OP_TSETB = 78, // B<VAR>[C<LIT>] = A<VAR>
	BC_OP_TSETM = 79, // A-1<BASE>[D&0xFFFFFFFF<NUM>] <- A (<- multres)
	BC_OP_TSETR = 0xFF40, // unsupported
	BC_OP_CALLM = 81, // if B<LIT> == 0 then A<BASE> (<- multres) <- A(A+FR2?2:1, A++ -> for C<LIT>, A++ (<- multres)) else A, A++ -> for B-1 = A(A+FR2?2:1, A++ -> for C, A++ (<- multres))
	BC_OP_CALL = 82, // if B<LIT> == 0 then A<BASE> (<- multres) <- A(A+FR2?2:1, A++ -> for C-1<LIT>) else A, A++ -> for B-1 = A(A+FR2?2:1, A++ -> for C-1)
	BC_OP_CALLMT = 83, // return A<BASE>(A+FR2?2:1, A++ -> for D<LIT>, A++ (<- multres))
	BC_OP_CALLT = 84, // return A<BASE>(A+FR2?2:1, A++ -> for D-1<LIT>)
	BC_OP_ITERC = 85, // for A<BASE>, A++ -> for B-1<LIT> in A-3, A-2, A-1 do
	BC_OP_ITERN = 86, // for A<BASE>, A++ -> for B-1<LIT> in A-3, A-2, A-1 do
	BC_OP_VARG = 87, // if B<LIT> == 0 then A<BASE> (<- multres) <- ... else A, A++ -> for B-1 = ...
	BC_OP_ISNEXT = 88, // goto ITERN at D<JUMP>
	BC_OP_RETM = 89, // return A<BASE>, A++ -> for D<LIT>, A++ (<- multres)
	BC_OP_RET = 90, // return A<RBASE>, A++ -> for D-1<LIT>
	BC_OP_RET0 = 91, // return
	BC_OP_RET1 = 92, // return A<RBASE>
	BC_OP_FORI = 93, // for A+3<BASE> = A, A+1, A+2 do; exit at D<JUMP>
	BC_OP_JFORI = 0xFF4E, // unsupported
	BC_OP_FORL = 95, // end of numeric for loop; start at D<JUMP>
	BC_OP_IFORL = 0xFF50, // unsupported
	BC_OP_JFORL = 0xFF51, // unsupported
	BC_OP_ITERL = 98, // end of generic for loop; start at D<JUMP>
	BC_OP_IITERL = 0xFF53, // unsupported
	BC_OP_JITERL = 0xFF54, // unsupported
	BC_OP_LOOP = 101, // if D<JUMP> == 32767 then goto loop else while/repeat loop; exit at D
	BC_OP_ILOOP = 0xFF56, // unsupported
	BC_OP_JLOOP = 0xFF57, // unsupported
	BC_OP_JMP = 104, // goto D<JUMP> or if true then JMP or goto ITERC at D
	BC_OP_FUNCF = 0xFF59, // unsupported
	BC_OP_IFUNCF = 0xFF5A, // unsupported
	BC_OP_JFUNCF = 0xFF5B, // unsupported
	BC_OP_FUNCV = 0xFF5C, // unsupported
	BC_OP_IFUNCV = 0xFF5D, // unsupported
	BC_OP_JFUNCV = 0xFF5E, // unsupported
	BC_OP_FUNCC = 0xFF5F, // unsupported
	BC_OP_FUNCCW = 0xFF60, // unsupported
	BC_OP_INVALID = 0xFFFF
};

struct Instruction {
	BC_OP type;
	uint8_t a = 0;
	uint8_t b = 0;
	uint8_t c = 0;
	uint16_t d = 0;
};

static BC_OP get_op_type(const uint8_t& byte, const uint8_t& version) {
	// 处理ISFC指令的特殊映射：12-20都映射到BC_OP_ISFC
	if (byte >= 12 && byte <= 20) {
		return BC_OP_ISFC;
	}
	// 其他指令直接返回字节码值
	return (BC_OP)byte;
}

static bool is_op_abc_format(const BC_OP& instruction) {
	switch (instruction) {
	case BC_OP_ADDVN:
	case BC_OP_SUBVN:
	case BC_OP_MULVN:
	case BC_OP_DIVVN:
	case BC_OP_MODVN:
	case BC_OP_ADDNV:
	case BC_OP_SUBNV:
	case BC_OP_MULNV:
	case BC_OP_DIVNV:
	case BC_OP_MODNV:
	case BC_OP_ADDVV:
	case BC_OP_SUBVV:
	case BC_OP_MULVV:
	case BC_OP_DIVVV:
	case BC_OP_MODVV:
	case BC_OP_POW:
	case BC_OP_CAT:
	case BC_OP_TGETV:
	case BC_OP_TGETS:
	case BC_OP_TGETB:
	case BC_OP_TGETR:
	case BC_OP_TSETV:
	case BC_OP_TSETS:
	case BC_OP_TSETB:
	case BC_OP_TSETR:
	case BC_OP_CALLM:
	case BC_OP_CALL:
	case BC_OP_ITERC:
	case BC_OP_ITERN:
	case BC_OP_VARG:
		return true;
	}

	return false;
}
