//���ͱ��������ͱ�

#include "cc.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>

Type *typeint, *typechar, *typenull;

static Type *ty, *tys;
static Type **argtyls, **argtylss;

void type_init(void) {
	tys = ty = (Type*)malloc(MAXSIZE * sizeof(Type));
	argtylss = argtyls = (Type**)malloc(MAXSIZE * sizeof(Type*));
	typeint = type_derive(INT, NULL, 0);
	typechar = type_derive(CHAR, NULL, 0);
	typenull = type_derive(NUL, NULL, 0);
}

static Type** getargtyls(int count) {
	if(count == 0) return NULL;
	Id *id = lid - count;
	Type **_argtyls = argtylss;
	int i = 0;
	while(_argtyls < argtyls) {
		i = 0;
		while(i < count && i < argtyls - _argtyls) {
			if(id[i].type != _argtyls[i]) break;
			i++;
		}
		if(i == count) return _argtyls;
		else if(i == argtyls - _argtyls) break;
		else _argtyls++;
	}
	while(i < count) {
		*argtyls++ = id[i++].type;
	}
	return _argtyls;
}

Type* type_derive(int base, Type *rely, int count) { //��������
	if(rely == NULL) {
		if(base == INT || base == CHAR || base == VOID || base == NUL) {
			for(Type *i = tys; i < ty; i++) {
				if(i->base == base
				&& i->rely == NULL) return i;
			}
			ty->base = base;
			ty->rely = NULL;
			return ty++;
		} else error("line %d: error!\n", line);
	} else {
		if(base == PTR) {
			for(Type *i = tys; i < ty; i++) {
				if(i->base == base
				&& i->rely == rely) return i;
			}
			ty->base = base;
			ty->rely = rely;
			return ty++;
		} else if(base == ARR) {
			if(rely->base == FUN || rely->base == VOID) error("line %d: error!\n", line);
			for(Type *i = tys; i < ty; i++) {
				if(i->base == base
				&& i->rely == rely
				&& i->count == count) return i;
			}
			ty->base = base;
			ty->rely = rely;
			ty->count = count;
			return ty++;
		} else if(base == FUN) {// || base == API) {
			if(rely->base == FUN || rely->base == ARR) error("line %d: error!\n", line);
			Type **argtyls = getargtyls(count);
			for(Type *i = tys; i < ty; i++) {
				if(i->base == base
				&& i->rely == rely
				&& i->count == count
				&& i->argtyls == argtyls) return i;
			}
			ty->base = base;
			ty->rely = rely;
			ty->count = count;
			ty->argtyls = argtyls;
			return ty++;
		} else error("line %d: error!\n", line);
	}
	return NULL; //make compiler happy
}

static void _print_type(Type *type) {
	if(type->base == PTR) {
		printf("ָ��");
		_print_type(type->rely);
		printf("��ָ��");
	} else if(type->base == ARR) {
		printf("ӵ��%d������Ϊ", type->count);
		_print_type(type->rely);
		printf("��Ԫ�ص�����");
	} else if(type->base == FUN) {
		for(int i = 0; i < type->count; i++) {
			printf("��%d������Ϊ", i + 1);
			_print_type(type->argtyls[i]);
			printf("��");
		}
		printf("����ֵΪ");
		_print_type(type->rely);
		printf("�ĺ���");
	} else if(type->base == API) {
		for(int i = 0; i < type->count; i++) {
			printf("��%d������Ϊ", i + 1);
			_print_type(type->argtyls[i]);
			printf("��");
		}
		printf("����ֵΪ");
		_print_type(type->rely);
		printf("��API");
	} else if(type->base == INT) {
		printf("����");
	} else if(type->base == CHAR) {
		printf("�ַ���");
	} else if(type->base == VOID) {
		printf("��");
	}
}

void print_type(Id *id) {
	printf("%sΪ", id->name);
	_print_type(id->type);
	//printf("\n");
}

int type_size(Type *type) {
	if(type->base == INT) return 1;
	else if(type->base == CHAR) return 1;
	else if(type->base == PTR) return 1;
	else if(type->base == ARR) return type_size(type->rely) * type->count;
	return 0;
}

void type_check(Type *type1, Type *type2, char *opr) {
	if(!strcmp(opr, "=")) {
		if(type1->base == INT || type1->base == CHAR) {
			if(type2->base == INT);
			else if(type2->base == CHAR);
			else error("line %d: error!\n", line);
		} else if(type1->base == PTR) {
			if(type1 == type2);
			else if(type2->base == NUL);
			else if(type2->base == FUN && type2 == type1->rely);
			else if(type2->base == ARR && type2->rely == type1->rely);
			else if(type2->base == PTR && type2->rely->base == VOID);
			else error("line %d: error!\n", line);
		}
	} else if(!strcmp(opr, "+") || !strcmp(opr, "-")) {
		if(type2->base == PTR || type2->base == ARR) {
			Type *tmp = type1; type1 = type2; type2 = tmp;
		}
		if(type1->base == INT || type1->base == CHAR || type1->base == PTR || type1->base == ARR) {
			if(type2->base == INT);
			else if(type2->base == CHAR);
			else error("line %d: error!\n", line);
		} else error("line %d: error!\n", line);
	} else if(!strcmp(opr, "*") || !strcmp(opr, "/") || !strcmp(opr, "%")) {
		if(type1->base == INT || type1->base == CHAR) {
			if(type2->base == INT);
			else if(type2->base == CHAR);
			else error("line %d: error!\n", line);
		} else error("line %d: error!\n", line);
	} else if(!strcmp(opr, "==") || !strcmp(opr, ">") || !strcmp(opr, "<") || !strcmp(opr, "!=") || !strcmp(opr, ">=") || !strcmp(opr, "<=")) {
		if(type1->base == INT || type1->base == CHAR) {
			if(type2->base == INT);
			else if(type2->base == CHAR);
			else error("line %d: error!\n", line);
		} else if(type1->base == PTR) {
			if(type1 == type2);
			else if(type2->base == NUL);
			else error("line %d: error!\n", line);
		} else if(type1->base == NUL) {
			if(type1 == type2);
			else if(type2->base == PTR);
			else error("line %d: error!\n", line);
		} else error("line %d: error!\n", line);
	} else if(!strcmp(opr, "&&") || !strcmp(opr, "||")) {
		if(type1->base == INT || type1->base == CHAR) {
			if(type2->base == INT);
			else if(type2->base == CHAR);
			else error("line %d: error!\n", line);
		} else error("line %d: error!\n", line);
	} else error("line %d: error!\n", line);
}
