//���ʽ����

#include "cc.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int const_ptr(Type *type) {
	if(tki == Null) {
		next();
		return 0;
	} else if(tki == STR) {
		if(type -> rely -> base == CHAR) {
			int offset = sgetstr(tks) -> offset;
			next();
			return offset;
		} else { printf("line %d: error23!\n", line); exit(-1); }
	} else { printf("line %d: error24!\n", line); exit(-1); }
}

void expr_arr(int scope, Type *type, int offset) {
	if(scope == GLO) {
		memset(data + offset, 0, type -> count);
		if(!strcmp(tks, "{")) {
			next();
			int count = 0;
			if(strcmp(tks, "}")) {
				while(1) {
					count++;
					if(type -> rely -> base == INT) data[offset] = const_expr("");
					else if(type -> rely -> base == CHAR) data[offset] = const_expr("");
					else if(type -> rely -> base == PTR) data[offset] = const_ptr(type -> rely);
					else if(type -> rely -> base == ARR) expr_arr(GLO, type -> rely, offset);
					else { printf("line %d: error25!\n", line); exit(-1); }
					
					if(!strcmp(tks, "}")) {
						break;
					} else if(!strcmp(tks, ",")) {
						offset += typesize(type -> rely);
						next();
					} else { printf("line %d: error26!\n", line); exit(-1); }
				}
			}
			if(type -> count < count) { printf("line %d: error27!\n", line); exit(-1); }
		} else if(tki == STR) {
			Id *strid = sgetstr(tks);
			if(type -> rely -> base != CHAR) { printf("line %d: error28!\n", line); exit(-1); }
			if(type -> count < strid -> type -> count) { printf("line %d: error29!\n", line); exit(-1); }
			for(int i = 0; i < strid -> type -> count; i++) {
				data[offset + i] = data[strid -> offset + i];
			}
		} else { printf("line %d: error30!\n", line); exit(-1); }
		next();
	} else if(scope == LOC) {
		if(!strcmp(tks, "{")) {
			next();
			int count = 0;
			if(strcmp(tks, "}")) {
				while(1) {
					count++;
					if(type -> rely -> base == INT || type -> rely -> base == CHAR || type -> rely -> base == PTR) {
						*e++ = AL; *e++ = offset;
						*e++ = PUSH; *e++ = AX;
						type_check(type -> rely, expr("").type, "=");
						*e++ = ASS;
					}
					else if(type -> rely -> base == ARR) expr_arr(LOC, type -> rely, offset);
					else { printf("line %d: error31!\n", line); exit(-1); }
					
					if(!strcmp(tks, "}")) {
						break;
					} else if(!strcmp(tks, ",")) {
						offset += typesize(type -> rely);
						next();
					} else { printf("line %d: error32!\n", line); exit(-1); }
				}
			}
			if(type -> count < count) { printf("line %d: error33!\n", line); exit(-1); }
		} else if(tki == STR) {
			Id *strid = sgetstr(tks);
			if(type -> rely -> base != CHAR) { printf("line %d: error34!\n", line); exit(-1); }
			if(type -> count < strid -> type -> count) { printf("line %d: error35!\n", line); exit(-1); }
			for(int i = 0; i < strid -> type -> count; i++) {
				*e++ = AL; *e++ = offset + i;
				*e++ = PUSH; *e++ = AX;
				*e++ = AG; *e++ = strid -> offset + i;
				*e++ = VAL;
				*e++ = ASS;
			}
		} else { printf("line %d: error36!\n", line); exit(-1); }
		next();
	}
}

static int lev(char *opr) { //���ȼ�Խ��levԽ����������levΪ0
	char *oprs[] = {
		")", "]",
		"", "&&", "||", "!",
		"", "==", "!=",
		"", ">", "<", ">=", "<=",
		"", "+", "-",
		"", "*", "/", "%",
		"", "=",
		"", "*_", "&_",
		"", "(", "["
	};
	int lev = 1;
	for(int i = 0; i < sizeof(oprs) / sizeof(*oprs); i++) {
		if(!strcmp(oprs[i], opr)) {
			return lev;
		} else if(!strcmp(oprs[i], "")) {
			lev++;
		}
	}
	return 0; //��������
}

int const_expr(char *last_opr) {
	int a;
	if(tki == INT) {
		a = atoi(tks);
		next();
	} else if(tki == CHAR) {
		a = tks[0];
	} else if(!strcmp(tks, "(")) {
		next();
		a = const_expr(")");
		if(strcmp(tks, ")")) { printf("line %d: error37!\n", line); exit(-1); } //"("�޷�ƥ�䵽")"
		next();
	} else if(!strcmp(tks, "!")) {
		next();
		a = !const_expr("!");
	} else if(!strcmp(tks, "-")) {
		next();
		a = -const_expr("-");
	} else { printf("line %d: error38!\n", line); exit(-1); }
	
	while(lev(tks) > lev(last_opr)) {
		char *opr = tks;
		next();
		int b = const_expr(opr);
		if (!strcmp(opr, "+")) a += b;
		else if(!strcmp(opr, "-")) a -= b;
		else if(!strcmp(opr, "*")) a *= b;
		else if(!strcmp(opr, "/")) a /= b;
		else if(!strcmp(opr, "%")) a %= b;
		else if(!strcmp(opr, ">")) a = a > b;
		else if(!strcmp(opr, "<")) a = a < b;
		else if(!strcmp(opr, "==")) a = a == b;
		else if(!strcmp(opr, ">=")) a = a >= b;
		else if(!strcmp(opr, "<=")) a = a <= b;
		else if(!strcmp(opr, "!=")) a = a != b;
		else if(!strcmp(opr, "&&")) a = a && b;
		else if(!strcmp(opr, "||")) a = a || b;
		else { printf("line %d: error39!\n", line); exit(-1); }
	}
	return a;
}

void type_check(Type *type1, Type *type2, char *opr) {
	if(!strcmp(opr, "=")) {
		if(type1 -> base == INT || type1 -> base == CHAR) {
			if(type2 -> base == INT);
			else if(type2 -> base == CHAR);
			else { printf("line %d: error40!\n", line); exit(-1); }
		} else if(type1 -> base == PTR) {
			if(type1 == type2);
			else if(type2 -> base == NUL);
			else if(type2 -> base == PTR && type2 -> rely -> base == VOID);
			else { printf("line %d: error41!\n", line); exit(-1); }
		}
	} else if(!strcmp(opr, "+") || !strcmp(opr, "-")) {
		if(type1 -> base == INT || type1 -> base == CHAR || type1 -> base == PTR || type1 -> base == ARR) {
			if(type2 -> base == INT);
			else if(type2 -> base == CHAR);
			else { printf("line %d: error42!\n", line); exit(-1); }
		} else { printf("line %d: error43!\n", line); exit(-1); }
	} else if(!strcmp(opr, "*") || !strcmp(opr, "/") || !strcmp(opr, "%")) {
		if(type1 -> base == INT || type1 -> base == CHAR) {
			if(type2 -> base == INT);
			else if(type2 -> base == CHAR);
			else { printf("line %d: error44!\n", line); exit(-1); }
		} else { printf("line %d: error45!\n", line); exit(-1); }
	} else if(!strcmp(opr, "==") || !strcmp(opr, ">") || !strcmp(opr, "<") || !strcmp(opr, "!=") || !strcmp(opr, ">=") || !strcmp(opr, "<=")) {
		if(type1 -> base == INT || type1 -> base == CHAR) {
			if(type2 -> base == INT);
			else if(type2 -> base == CHAR);
			else { printf("line %d: error46!\n", line); exit(-1); }
		} else if(type1 -> base == PTR) {
			if(type1 == type2);
			else if(type2 -> base == NUL);
			else { printf("line %d: error47!\n", line); exit(-1); }
		} else if(type1 -> base == NUL) {
			if(type1 == type2);
			else if(type2 -> base == PTR);
			else { printf("line %d: error48!\n", line); exit(-1); }
		} else { printf("line %d: error49!\n", line); exit(-1); }
	} else if(!strcmp(opr, "&&") || !strcmp(opr, "||")) {
		if(type1 -> base == INT || type1 -> base == CHAR) {
			if(type2 -> base == INT);
			else if(type2 -> base == CHAR);
			else { printf("line %d: error50!\n", line); exit(-1); }
		} else { printf("line %d: error51!\n", line); exit(-1); }
	} else { printf("line %d: error52!\n", line); exit(-1); }
}

Er expr(char *last_opr) { //1 + 2 ^ 3 * 4 == (1 + (2 ^ (3) * (4)))
	Er er = {NULL, 0, 1};
	if(tki == INT) {
		er.type = deriv_type(INT, NULL, 0);
		*e++ = SET; *e++ = AX; *e++ = atoi(tks);
		next();
	} else if(tki == CHAR) {
		er.type = deriv_type(CHAR, NULL, 0);
		*e++ = SET; *e++ = AX; *e++ = tks[0];
		next();
	} else if(tki == NUL) {
		er.type = deriv_type(NUL, NULL, 0);
		*e++ = SET; *e++ = AX; *e++ = 0;
		next();
	} else if(tki == STR) {
		Id *this_id = sgetstr(tks);
		*e++ = AG; *e++ = this_id -> offset;
		er.type = this_id -> type;
		if(strcmp(last_opr, "&")) er.type = deriv_type(PTR, er.type -> rely, 0);
		er.is_lvalue = 1;
		next();
	} else if(tki == ID) {
		Id *this_id = getid(tks);
		er.type = this_id -> type;
		*e++ = this_id -> class == GLO ? AG: AL; *e++ = this_id -> offset;
		if(er.type -> base == INT || er.type -> base == CHAR || er.type -> base == PTR) {
			*e++ = VAL;
			er.is_const = 0;
		}
		er.is_lvalue = 1;
		next();
	} else if(!strcmp(tks, "(")) {
		next();
		er.type = expr(")").type;
		if(strcmp(tks, ")")) { printf("line %d: error53!\n", line); exit(-1); }
		next();
	} else if(!strcmp(tks, "*")) {
		next();
		er.type = expr("*_").type;
		if(er.type -> base != PTR) { printf("line %d: error54!\n", line); exit(-1); }
		er.type = er.type -> rely;
		if(er.type -> base == INT || er.type -> base == CHAR || er.type -> base == PTR) {
			*e++ = VAL;
			er.is_const = 0;
		}
		er.is_lvalue = 1;
	} else if(!strcmp(tks, "&")) {
		next();
		Er _er = expr("&_");
		if(!_er.is_lvalue) { printf("line %d: error55!\n", line); exit(-1); }
		if(_er.type -> base == INT || _er.type -> base == PTR) e--;
		er.type = deriv_type(PTR, _er.type, 0);
	} else if(!strcmp(tks, "!")) {
		next();
		er.type = expr("!").type;
		if(er.type -> base != INT) er.type = deriv_type(INT, NULL, 0);
		*e++ = NOT;
	} else if(!strcmp(tks, "-")) {
		next();
		*e++ = SET; *e++ = AX; *e++ = 0;
		*e++ = PUSH; *e++ = AX;
		er.type = expr("-").type;
		if(er.type -> base == CHAR) er.type = deriv_type(INT, NULL, 0);
		else if(er.type -> base != INT) { printf("line %d: error56!\n", line); exit(-1); }
		*e++ = SUB;
	} else { printf("line %d: error57!\n", line); exit(-1); }
	
	while(lev(tks) > lev(last_opr)) {
		*e++ = PUSH; *e++ = AX;
		if(!strcmp(tks, "=")) {
			next();
			e -= 3; *e++ = PUSH; *e++ = AX;
			type_check(er.type, expr("").type, "=");
			if(!er.is_const) *e++ = ASS; else { printf("line %d: error58!\n", line); exit(-1); }
		} else if(!strcmp(tks, "(")) {
			next();
			int argc = 0;
			if(er.type -> base == PTR) er.type = er.type -> rely; //������ָ��ת��Ϊ����
			if(er.type -> base != FUN && er.type -> base != API) { printf("line %d: error59!\n", line); exit(-1); }
			if(strcmp(tks, ")")) {
				while(1) {
					if(argc > er.type -> count) { printf("line %d: error60!\n", line); exit(-1); } //��������
					Type *argtype = expr(")").type;
					if(argtype -> base == FUN) argtype = deriv_type(PTR, argtype, 0);
					else if(argtype -> base == ARR) argtype = deriv_type(PTR, argtype -> rely, 0);
					type_check((er.type -> argtyls)[argc], argtype, "="); //�������ͼ��
					*e++ = PUSH; *e++ = AX;
					argc++;
					if(!strcmp(tks, ")")) break;
					else if(!strcmp(tks, ",")) next();
					else { printf("line %d: error61!\n", line); exit(-1); }
				}
			}
			next();
			if(argc < er.type -> count) { printf("line %d: error62!\n", line); exit(-1); } //��������
			if(er.type -> base == FUN) {
				*e++ = SET; *e++ = AX; int *_e = e++; //set next ip
				*e++ = PUSH; *e++ = AX;
				*e++ = CALL; *e++ = argc;
				*_e = e - emit;
				*e++ = DEC; *e++ = SP; *e++ = argc + 1;
			} else if(er.type -> base == API) {
				*e++ = CAPI; *e++ = argc;
				*e++ = DEC; *e++ = SP; *e++ = argc + 1;
			}
			er.type = er.type -> rely;
		} else if(!strcmp(tks, "[")) {
			next();
			if(er.type -> base == PTR || er.type -> base == ARR) er.type = er.type -> rely;
			else { printf("line %d: error63!\n", line); exit(-1); }
			Type *type = expr("]").type;
			if(type -> base != INT && type -> base != CHAR) { printf("line %d: error64!\n", line); exit(-1); }
			*e++ = PUSH; *e++ = AX;
			*e++ = SET; *e++ = AX; *e++ = typesize(er.type);
			*e++ = MUL;
			*e++ = ADD;
			if(er.type -> base == INT || er.type -> base == CHAR || er.type -> base == PTR) {
				*e++ = VAL;
				er.is_const = 0;
			}
			er.is_lvalue = 1;
			next();
		} else if(!strcmp(tks, "+")) {
			next();
			type_check(er.type, expr("").type, "+");
			if(er.type -> base == INT) {
				*e++ = ADD;
			} else if(er.type -> base == PTR || er.type -> base == ARR) {
				*e++ = PUSH; *e++ = AX;
				*e++ = SET; *e++ = AX; *e++ = typesize(er.type -> rely);
				*e++ = MUL;
				*e++ = ADD;
				if(er.type -> base == ARR) er.type = deriv_type(PTR, er.type -> rely, 0);
			} else { printf("line %d: error65!\n", line); exit(-1); }
		} else if(!strcmp(tks, "-")) {
			next();
			type_check(er.type, expr("").type, "-");
			if(er.type -> base == INT) {
				*e++ = SUB;
			} else if(er.type -> base == PTR || er.type -> base == ARR) {
				*e++ = PUSH; *e++ = AX;
				*e++ = SET; *e++ = AX; *e++ = typesize(er.type -> rely);
				*e++ = MUL;
				*e++ = SUB;
				if(er.type -> base == ARR) er.type = deriv_type(PTR, er.type -> rely, 0);
			} else { printf("line %d: error66!\n", line); exit(-1); }
		} else {
			char *opr = tks;
			next();
			type_check(er.type, expr(opr).type, opr);
			er.type = deriv_type(INT, NULL, 0);
			if(!strcmp(opr, "*")) *e++ = MUL;
			else if(!strcmp(opr, "/")) *e++ = DIV;
			else if(!strcmp(opr, "%")) *e++ = MOD;
			else if(!strcmp(opr, "==")) *e++ = EQ;
			else if(!strcmp(opr, ">")) *e++ = GT;
			else if(!strcmp(opr, "<")) *e++ = LT;
			else if(!strcmp(opr, "!=")) { *e++ = EQ; *e++ = NOT; }
			else if(!strcmp(opr, ">=")) { *e++ = LT; *e++ = NOT; }
			else if(!strcmp(opr, "<=")) { *e++ = GT; *e++ = NOT; }
			else if(!strcmp(opr, "&&")) *e++ = AND;
			else if(!strcmp(opr, "||")) *e++ = OR;
			else { printf("line %d: error67!\n", line); exit(-1); }
		}
	}
	return er;
}
