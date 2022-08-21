#include "mycc.h"

int label_count = 0;

void gen_lval(Node *node)
{
	if (node->kind != ND_LVAR)
		error("left value of assignment must be variable");

	printf("	mov rax, rbp\n");
	printf("	sub rax, %d\n", node->offset);
	printf("	push rax\n");
}

void gen(Node *node)
{
	int l;
	Node *n;

	switch (node->kind)
	{
	case ND_NUM:
		printf("  push %d\n", node->val);
		return;
	case ND_LVAR:
		gen_lval(node);
		printf("	pop rax\n");
		printf("	mov rax, [rax]\n");
		printf("	push rax\n");
		return;
	case ND_ASSIGN:
		gen_lval(node->lhs);
		gen(node->rhs);

		printf("	pop rdi\n");
		printf("	pop rax\n");
		printf("	mov [rax], rdi\n");
		printf("	push rdi\n");
		return;
	case ND_RETURN:
		gen(node->lhs);
		printf("	pop rax\n");
		printf("	mov rsp, rbp\n");
		printf("	pop rbp\n");
		printf("	ret\n");
		return;
	case ND_IF:
		l = label_count++;
		gen(node->lhs);
		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je .Lend%d\n", l);
		gen(node->rhs);
		printf(".Lend%d:\n", l);
		return;
	case ND_IFELSE:
		l = label_count++;
		gen(node->lhs);
		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je .Lelse%d\n", l);
		gen(node->rhs->lhs);
		printf("	jmp .Lend%d\n", l);
		printf(".Lelse%d:\n", l);
		gen(node->rhs->rhs);
		printf(".Lend%d:\n", l);
		return;
	case ND_WHILE:
		l = label_count++;
		printf(".Lbegin%d:\n", l);
		gen(node->lhs);
		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je .Lend%d\n", l);
		gen(node->rhs);
		printf("	jmp .Lbegin%d\n", l);
		printf(".Lend%d:\n", l);
		return;
	case ND_FOR:
		l = label_count++;
		gen(node->lhs->lhs);
		printf(".Lbegin%d:\n", l);
		gen(node->lhs->rhs);
		printf("	pop rax\n");
		printf("	cmp rax, 0\n");
		printf("	je .Lend%d\n", l);
		gen(node->rhs->rhs);
		gen(node->rhs->lhs);
		printf("	jmp .Lbegin%d\n", l);
		printf(".Lend%d:\n", l);
		return;
	case ND_BLOCK:
		n = node->rhs;
		while (n)
		{
			gen(n->lhs);
			n = n->rhs;
		}
		return;
	}

	gen(node->lhs);
	gen(node->rhs);

	printf("  pop rdi\n");
	printf("  pop rax\n");

	switch (node->kind)
	{
	case ND_ADD:
		printf("  add rax, rdi\n");
		break;
	case ND_SUB:
		printf("  sub rax, rdi\n");
		break;
	case ND_MUL:
		printf("  imul rax, rdi\n");
		break;
	case ND_DIV:
		printf("  cqo\n");
		printf("  idiv rdi\n");
		break;
	case ND_EQ:
		printf("  cmp rax, rdi\n");
		printf("  sete al\n");
		printf("  movzb rax, al\n");
		break;
	case ND_NE:
		printf("  cmp rax, rdi\n");
		printf("  setne al\n");
		printf("  movzb rax, al\n");
		break;
	case ND_LT:
		printf("  cmp rax, rdi\n");
		printf("  setl al\n");
		printf("  movzb rax, al\n");
		break;
	case ND_LE:
		printf("  cmp rax, rdi\n");
		printf("  setle al\n");
		printf("  movzb rax, al\n");
		break;
	}

	printf("  push rax\n");
}