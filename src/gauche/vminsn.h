/*
 * vminsn.h - Virtual machine instruction definition
 *
 *  Copyright(C) 2000-2002 by Shiro Kawai (shiro@acm.org)
 *
 *  Permission to use, copy, modify, distribute this software and
 *  accompanying documentation for any purpose is hereby granted,
 *  provided that existing copyright notices are retained in all
 *  copies and that this notice is included verbatim in all
 *  distributions.
 *  This software is provided as is, without express or implied
 *  warranty.  In no circumstances the author(s) shall be liable
 *  for any damages arising out of the use of this software.
 *
 *  $Id: vminsn.h,v 1.33 2002-11-05 22:55:28 shirok Exp $
 */

/* DEFINSN(symbol, name, # of parameters) */

/* NOP
 *   Input stack  : -
 *   Result stack : -
 *  Used for placeholder.
 */
DEFINSN(SCM_VM_NOP, "NOP", 0)

/* PUSH
 *
 *  Push value of val0 to the stack top
 */
DEFINSN(SCM_VM_PUSH, "PUSH", 0)

/* combined push */
DEFINSN(SCM_VM_PUSHI, "PUSHI", 1) /* push immediate integer */
DEFINSN(SCM_VM_PUSHNIL, "PUSHNIL", 0) /* push '() */
    
/* POP
 *
 *  Pop arg
 */
DEFINSN(SCM_VM_POP, "POP", 0)

/* DUP
 *
 *  Duplicate the value on top of the stack
 */
DEFINSN(SCM_VM_DUP, "DUP", 0)

/* PRE-CALL(nargs)
 *
 *  Prepare for a normal call.
 */
DEFINSN(SCM_VM_PRE_CALL, "PRE-CALL", 1)

/* PRE-TAIL(nargs)
 *
 *  Prepare for a tail call.
 */
DEFINSN(SCM_VM_PRE_TAIL, "PRE-TAIL", 1)

/* CHECK-STACK(size)
 *
 *  Check for stack overflow
 */
DEFINSN(SCM_VM_CHECK_STACK, "CHECK-STACK", 1)

/* CALL(NARGS)
 *
 *  Call procedure in val0.
 */
DEFINSN(SCM_VM_CALL, "CALL", 1)

/* TAIL-CALL(NARGS)
 *
 *  Call procedure in val0.
 */
DEFINSN(SCM_VM_TAIL_CALL, "TAIL-CALL", 1)

/* DEFINE <SYMBOL>
 *
 *  Defines global binding of SYMBOL in the current module.
 *  The value is taken from the input stack.
 *  This instruction only appears at the toplevel.  Internal defines
 *  are recognized and eliminated by the compiling process.
 */
DEFINSN(SCM_VM_DEFINE, "DEFINE", 0)
DEFINSN(SCM_VM_DEFINE_CONST, "DEFINE-CONST", 0)

/* LAMBDA(NARGS,RESTARG) <CODE>
 *
 *  Create a closure capturing current environment.
 *  CODE is the compiled code.   Leaves created closure in the stack.
 */
DEFINSN(SCM_VM_LAMBDA, "LAMBDA", 2)

/* LET(NLOCALS) <BODY>
 *
 *  Create a new environment frame, size of NLOCALS.  let-families
 *  like let, let* and letrec yields this instruction.
 */
DEFINSN(SCM_VM_LET, "LET", 1)

/* IF  <THEN-CODE>
 *
 *  If val0 is true, transfer control to THEN-CODE.  Otherwise
 *  it continues execution.   Test arg is popped.
 */
DEFINSN(SCM_VM_IF, "IF", 0)

/* TAILBIND(NARGS)
 *
 *  Lightweight tail call.  This instruction appears in the loop body
 *  and the tail call to inlined procedures.
 */
DEFINSN(SCM_VM_TAILBIND, "TAILBIND", 1)

/* VALUES-BIND(NARGS,RESTARG) <BODY> ...
 *
 *  Primitive operation for receive and call-with-values.
 *  Turn the multiple values into an environment, then evaluate <BODY> ...
 */
DEFINSN(SCM_VM_VALUES_BIND, "VALUES-BIND", 2)

/* LSET(DEPTH, OFFSET)
 *
 *  Local set
 */
DEFINSN(SCM_VM_LSET, "LSET", 2)

/* shortcut for the first frame, small offset */
DEFINSN(SCM_VM_LSET0, "LSET0", 0)
DEFINSN(SCM_VM_LSET1, "LSET1", 0)
DEFINSN(SCM_VM_LSET2, "LSET2", 0)
DEFINSN(SCM_VM_LSET3, "LSET3", 0)
DEFINSN(SCM_VM_LSET4, "LSET4", 0)

/* GSET <LOCATION>
 *
 *  LOCATION may be a symbol or gloc
 */
DEFINSN(SCM_VM_GSET, "GSET", 0)

/* LREF(DEPTH,OFFSET)
 *
 *  Retrieve local value.
 */
DEFINSN(SCM_VM_LREF, "LREF", 2)

/* shortcut for the first and second frame, small offset */
DEFINSN(SCM_VM_LREF0, "LREF0", 0)
DEFINSN(SCM_VM_LREF1, "LREF1", 0)
DEFINSN(SCM_VM_LREF2, "LREF2", 0)
DEFINSN(SCM_VM_LREF3, "LREF3", 0)
DEFINSN(SCM_VM_LREF4, "LREF4", 0)

DEFINSN(SCM_VM_LREF10, "LREF10", 0)
DEFINSN(SCM_VM_LREF11, "LREF11", 0)
DEFINSN(SCM_VM_LREF12, "LREF12", 0)
DEFINSN(SCM_VM_LREF13, "LREF13", 0)
DEFINSN(SCM_VM_LREF14, "LREF14", 0)

/* combined instrction */
DEFINSN(SCM_VM_LREF_PUSH, "LREF-PUSH", 2)
DEFINSN(SCM_VM_LREF0_PUSH, "LREF0-PUSH", 0)
DEFINSN(SCM_VM_LREF1_PUSH, "LREF1-PUSH", 0)
DEFINSN(SCM_VM_LREF2_PUSH, "LREF2-PUSH", 0)
DEFINSN(SCM_VM_LREF3_PUSH, "LREF3-PUSH", 0)
DEFINSN(SCM_VM_LREF4_PUSH, "LREF4-PUSH", 0)
DEFINSN(SCM_VM_LREF10_PUSH, "LREF10-PUSH", 0)
DEFINSN(SCM_VM_LREF11_PUSH, "LREF11-PUSH", 0)
DEFINSN(SCM_VM_LREF12_PUSH, "LREF12-PUSH", 0)
DEFINSN(SCM_VM_LREF13_PUSH, "LREF13-PUSH", 0)
DEFINSN(SCM_VM_LREF14_PUSH, "LREF14-PUSH", 0)


/* GREF <LOCATION>
 *
 *  LOCATION may be a symbol or GLOC object.
 *  Retrieve global value in the current module.
 */
DEFINSN(SCM_VM_GREF, "GREF", 0)
/*DEFINSN(SCM_VM_GREF_PUSH, "GREF-PUSH", 0)*/

/* PROMISE
 *
 *  Delay syntax emits this instruction.  Wrap a procedure into a promise
 *  object.
 */
DEFINSN(SCM_VM_PROMISE, "PROMISE", 0)

/* QUOTE-INSN <INSN>
 *
 *  Quote the next VM instruction.  Needs to load VM insn to val0.
 *  It occurs when VM insn is passed to apply.
 */
DEFINSN(SCM_VM_QUOTE_INSN, "QUOTE-INSN", 0)

/* Only used in NVM code */
#ifdef GAUCHE_USE_NVM
/* CONST <VALUE>
 *   Load <VALUE> to the register.
 */
DEFINSN(SCM_VM_CONST, "CONST", 0)

/* RET
 *   Pop the topmost continuation frame
 */
DEFINSN(SCM_VM_RET, "RET", 0)

/* JUMP <NEXT>
 *   Transfer the control to <NEXT>
 */
DEFINSN(SCM_VM_JUMP, "JUMP", 0)
#endif /*GAUCHE_USE_NVM*/

/* Inlined operators
 *  They work the same as corresponding Scheme primitives, but they are
 *  directly interpreted by VM, skipping argument processing part.
 *  Compiler may insert these in order to fulfill the operation (e.g.
 *  `case' needs MEMV).  If the optimization level is high, global
 *  reference of those primitive calls in the user code are replaced
 *  as well.
 */
DEFINSN(SCM_VM_CONS, "CONS", 0)
DEFINSN(SCM_VM_CONS_PUSH, "CONS-PUSH", 0)
DEFINSN(SCM_VM_CAR, "CAR", 0)
DEFINSN(SCM_VM_CAR_PUSH, "CAR-PUSH", 0)
DEFINSN(SCM_VM_CDR, "CDR", 0)
DEFINSN(SCM_VM_CDR_PUSH, "CDR-PUSH", 0)
DEFINSN(SCM_VM_LIST, "LIST", 1)
/*DEFINSN(SCM_VM_LIST_PUSH, "LIST-PUSH", 0)*/
DEFINSN(SCM_VM_LIST_STAR, "LIST*", 1)
DEFINSN(SCM_VM_MEMQ, "MEMQ", 0)
DEFINSN(SCM_VM_MEMV, "MEMV", 0)
DEFINSN(SCM_VM_ASSQ, "ASSQ", 0)
DEFINSN(SCM_VM_ASSV, "ASSV", 0)
DEFINSN(SCM_VM_EQ, "EQ?", 0)
DEFINSN(SCM_VM_EQV, "EQV?", 0)
DEFINSN(SCM_VM_APPEND, "APPEND", 1)
DEFINSN(SCM_VM_NOT, "NOT", 0)
DEFINSN(SCM_VM_REVERSE, "REVERSE", 0)
DEFINSN(SCM_VM_APPLY, "APPLY", 1)
/*DEFINSN(SCM_VM_NOT_NULLP, "NOT-NULL?", 0)*/
/*DEFINSN(SCM_VM_FOR_EACH, "FOR-EACH", 1)*/
/*DEFINSN(SCM_VM_MAP, "MAP", 1)*/

DEFINSN(SCM_VM_NULLP, "NULL?", 0)
DEFINSN(SCM_VM_PAIRP, "PAIR?", 0)
DEFINSN(SCM_VM_CHARP, "CHAR?", 0)
DEFINSN(SCM_VM_EOFP,  "EOF?", 0)
DEFINSN(SCM_VM_STRINGP, "STRING?", 0)
DEFINSN(SCM_VM_SYMBOLP, "SYMBOL?", 0)

DEFINSN(SCM_VM_SETTER, "SETTER", 0)

DEFINSN(SCM_VM_VALUES, "VALUES", 1)

DEFINSN(SCM_VM_VEC, "VEC", 1)
DEFINSN(SCM_VM_APP_VEC, "APP-VEC", 1)
DEFINSN(SCM_VM_VEC_LEN, "VEC-LEN", 0)
DEFINSN(SCM_VM_VEC_REF, "VEC-REF", 0)
DEFINSN(SCM_VM_VEC_SET, "VEC-SET", 0)

DEFINSN(SCM_VM_NUMEQ2, "NUMEQ2", 0)
DEFINSN(SCM_VM_NUMLT2, "NUMLT2", 0)
DEFINSN(SCM_VM_NUMLE2, "NUMLE2", 0)
DEFINSN(SCM_VM_NUMGT2, "NUMGT2", 0)
DEFINSN(SCM_VM_NUMGE2, "NUMGE2", 0)
DEFINSN(SCM_VM_NUMADD2, "NUMADD2", 0)
DEFINSN(SCM_VM_NUMSUB2, "NUMSUB2", 0)

DEFINSN(SCM_VM_NUMADDI, "NUMADDI", 1)
DEFINSN(SCM_VM_NUMSUBI, "NUMSUBI", 1)

DEFINSN(SCM_VM_READ_CHAR, "READ-CHAR", 1)
DEFINSN(SCM_VM_WRITE_CHAR, "WRITE-CHAR", 1)

DEFINSN(SCM_VM_SLOT_REF, "SLOT-REF", 0)
DEFINSN(SCM_VM_SLOT_SET, "SLOT-SET", 0)
