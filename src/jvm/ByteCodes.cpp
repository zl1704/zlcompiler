/*
 * ByteCodes.cpp
 *
 *  Created on: 2017年12月31日
 *      Author: ZL
 */

#include "ByteCodes.hpp"
string ByteCodes::typecodeNames[]={
        "int",
        "long",
        "float",
        "double",
        "object",
        "byte",
        "char",
        "short",
        "void",
        "oops"
    };
string ByteCodes::mnem[nop] = "nop";
string ByteCodes::mnem[aconst_null] = "aconst_null";
string ByteCodes::mnem[iconst_m1] = "iconst_m1";
string ByteCodes::mnem[iconst_0] = "iconst_0";
string ByteCodes::mnem[iconst_1] = "iconst_1";
string ByteCodes::mnem[iconst_2] = "iconst_2";
string ByteCodes::mnem[iconst_3] = "iconst_3";
string ByteCodes::mnem[iconst_4] = "iconst_4";
string ByteCodes::mnem[iconst_5] = "iconst_5";
string ByteCodes::mnem[lconst_0] = "lconst_0";
string ByteCodes::mnem[lconst_1] = "lconst_1";
string ByteCodes::mnem[fconst_0] = "fconst_0";
string ByteCodes::mnem[fconst_1] = "fconst_1";
string ByteCodes::mnem[fconst_2] = "fconst_2";
string ByteCodes::mnem[dconst_0] = "dconst_0";
string ByteCodes::mnem[dconst_1] = "dconst_1";
string ByteCodes::mnem[bipush] = "bipush";
string ByteCodes::mnem[sipush] = "sipush";
string ByteCodes::mnem[ldc1] = "ldc1";
string ByteCodes::mnem[ldc2] = "ldc2";
string ByteCodes::mnem[ldc2w] = "ldc2w";
string ByteCodes::mnem[iload] = "iload";
string ByteCodes::mnem[lload] = "lload";
string ByteCodes::mnem[fload] = "fload";
string ByteCodes::mnem[dload] = "dload";
string ByteCodes::mnem[aload] = "aload";
string ByteCodes::mnem[iload_0] = "iload_0";
string ByteCodes::mnem[lload_0] = "lload_0";
string ByteCodes::mnem[fload_0] = "fload_0";
string ByteCodes::mnem[dload_0] = "dload_0";
string ByteCodes::mnem[aload_0] = "aload_0";
string ByteCodes::mnem[iload_1] = "iload_1";
string ByteCodes::mnem[lload_1] = "lload_1";
string ByteCodes::mnem[fload_1] = "fload_1";
string ByteCodes::mnem[dload_1] = "dload_1";
string ByteCodes::mnem[aload_1] = "aload_1";
string ByteCodes::mnem[iload_2] = "iload_2";
string ByteCodes::mnem[lload_2] = "lload_2";
string ByteCodes::mnem[fload_2] = "fload_2";
string ByteCodes::mnem[dload_2] = "dload_2";
string ByteCodes::mnem[aload_2] = "aload_2";
string ByteCodes::mnem[iload_3] = "iload_3";
string ByteCodes::mnem[lload_3] = "lload_3";
string ByteCodes::mnem[fload_3] = "fload_3";
string ByteCodes::mnem[dload_3] = "dload_3";
string ByteCodes::mnem[aload_3] = "aload_3";
string ByteCodes::mnem[iaload] = "iaload";
string ByteCodes::mnem[laload] = "laload";
string ByteCodes::mnem[faload] = "faload";
string ByteCodes::mnem[daload] = "daload";
string ByteCodes::mnem[aaload] = "aaload";
string ByteCodes::mnem[baload] = "baload";
string ByteCodes::mnem[caload] = "caload";
string ByteCodes::mnem[saload] = "saload";
string ByteCodes::mnem[istore] = "istore";
string ByteCodes::mnem[lstore] = "lstore";
string ByteCodes::mnem[fstore] = "fstore";
string ByteCodes::mnem[dstore] = "dstore";
string ByteCodes::mnem[astore] = "astore";
string ByteCodes::mnem[istore_0] = "istore_0";
string ByteCodes::mnem[lstore_0] = "lstore_0";
string ByteCodes::mnem[fstore_0] = "fstore_0";
string ByteCodes::mnem[dstore_0] = "dstore_0";
string ByteCodes::mnem[astore_0] = "astore_0";
string ByteCodes::mnem[istore_1] = "istore_1";
string ByteCodes::mnem[lstore_1] = "lstore_1";
string ByteCodes::mnem[fstore_1] = "fstore_1";
string ByteCodes::mnem[dstore_1] = "dstore_1";
string ByteCodes::mnem[astore_1] = "astore_1";
string ByteCodes::mnem[istore_2] = "istore_2";
string ByteCodes::mnem[lstore_2] = "lstore_2";
string ByteCodes::mnem[fstore_2] = "fstore_2";
string ByteCodes::mnem[dstore_2] = "dstore_2";
string ByteCodes::mnem[astore_2] = "astore_2";
string ByteCodes::mnem[istore_3] = "istore_3";
string ByteCodes::mnem[lstore_3] = "lstore_3";
string ByteCodes::mnem[fstore_3] = "fstore_3";
string ByteCodes::mnem[dstore_3] = "dstore_3";
string ByteCodes::mnem[astore_3] = "astore_3";
string ByteCodes::mnem[iastore] = "iastore";
string ByteCodes::mnem[lastore] = "lastore";
string ByteCodes::mnem[fastore] = "fastore";
string ByteCodes::mnem[dastore] = "dastore";
string ByteCodes::mnem[aastore] = "aastore";
string ByteCodes::mnem[bastore] = "bastore";
string ByteCodes::mnem[castore] = "castore";
string ByteCodes::mnem[sastore] = "sastore";
string ByteCodes::mnem[pop] = "pop";
string ByteCodes::mnem[pop2] = "pop2";
string ByteCodes::mnem[dup] = "dup";
string ByteCodes::mnem[dup_x1] = "dup_x1";
string ByteCodes::mnem[dup_x2] = "dup_x2";
string ByteCodes::mnem[dup2] = "dup2";
string ByteCodes::mnem[dup2_x1] = "dup2_x1";
string ByteCodes::mnem[dup2_x2] = "dup2_x2";
string ByteCodes::mnem[swap] = "swap";
string ByteCodes::mnem[iadd] = "iadd";
string ByteCodes::mnem[ladd] = "ladd";
string ByteCodes::mnem[fadd] = "fadd";
string ByteCodes::mnem[dadd] = "dadd";
string ByteCodes::mnem[isub] = "isub";
string ByteCodes::mnem[lsub] = "lsub";
string ByteCodes::mnem[fsub] = "fsub";
string ByteCodes::mnem[dsub] = "dsub";
string ByteCodes::mnem[imul] = "imul";
string ByteCodes::mnem[lmul] = "lmul";
string ByteCodes::mnem[fmul] = "fmul";
string ByteCodes::mnem[dmul] = "dmul";
string ByteCodes::mnem[idiv] = "idiv";
string ByteCodes::mnem[ldiv] = "ldiv";
string ByteCodes::mnem[fdiv] = "fdiv";
string ByteCodes::mnem[ddiv] = "ddiv";
string ByteCodes::mnem[imod] = "imod";
string ByteCodes::mnem[lmod] = "lmod";
string ByteCodes::mnem[fmod] = "fmod";
string ByteCodes::mnem[dmod] = "dmod";
string ByteCodes::mnem[ineg] = "ineg";
string ByteCodes::mnem[lneg] = "lneg";
string ByteCodes::mnem[fneg] = "fneg";
string ByteCodes::mnem[dneg] = "dneg";
string ByteCodes::mnem[ishl] = "ishl";
string ByteCodes::mnem[lshl] = "lshl";
string ByteCodes::mnem[ishr] = "ishr";
string ByteCodes::mnem[lshr] = "lshr";
string ByteCodes::mnem[iushr] = "iushr";
string ByteCodes::mnem[lushr] = "lushr";
string ByteCodes::mnem[iand] = "iand";
string ByteCodes::mnem[land] = "land";
string ByteCodes::mnem[ior] = "ior";
string ByteCodes::mnem[lor] = "lor";
string ByteCodes::mnem[ixor] = "ixor";
string ByteCodes::mnem[lxor] = "lxor";
string ByteCodes::mnem[iinc] = "iinc";
string ByteCodes::mnem[i2l] = "i2l";
string ByteCodes::mnem[i2f] = "i2f";
string ByteCodes::mnem[i2d] = "i2d";
string ByteCodes::mnem[l2i] = "l2i";
string ByteCodes::mnem[l2f] = "l2f";
string ByteCodes::mnem[l2d] = "l2d";
string ByteCodes::mnem[f2i] = "f2i";
string ByteCodes::mnem[f2l] = "f2l";
string ByteCodes::mnem[f2d] = "f2d";
string ByteCodes::mnem[d2i] = "d2i";
string ByteCodes::mnem[d2l] = "d2l";
string ByteCodes::mnem[d2f] = "d2f";
string ByteCodes::mnem[int2byte] = "int2byte";
string ByteCodes::mnem[int2char] = "int2char";
string ByteCodes::mnem[int2short] = "int2short";
string ByteCodes::mnem[lcmp] = "lcmp";
string ByteCodes::mnem[fcmpl] = "fcmpl";
string ByteCodes::mnem[fcmpg] = "fcmpg";
string ByteCodes::mnem[dcmpl] = "dcmpl";
string ByteCodes::mnem[dcmpg] = "dcmpg";
string ByteCodes::mnem[ifeq] = "ifeq";
string ByteCodes::mnem[ifne] = "ifne";
string ByteCodes::mnem[iflt] = "iflt";
string ByteCodes::mnem[ifge] = "ifge";
string ByteCodes::mnem[ifgt] = "ifgt";
string ByteCodes::mnem[ifle] = "ifle";
string ByteCodes::mnem[if_icmpeq] = "if_icmpeq";
string ByteCodes::mnem[if_icmpne] = "if_icmpne";
string ByteCodes::mnem[if_icmplt] = "if_icmplt";
string ByteCodes::mnem[if_icmpge] = "if_icmpge";
string ByteCodes::mnem[if_icmpgt] = "if_icmpgt";
string ByteCodes::mnem[if_icmple] = "if_icmple";
string ByteCodes::mnem[if_acmpeq] = "if_acmpeq";
string ByteCodes::mnem[if_acmpne] = "if_acmpne";
string ByteCodes::mnem[goto_] = "goto_";
string ByteCodes::mnem[jsr] = "jsr";
string ByteCodes::mnem[ret] = "ret";
string ByteCodes::mnem[tableswitch] = "tableswitch";
string ByteCodes::mnem[lookupswitch] = "lookupswitch";
string ByteCodes::mnem[ireturn] = "ireturn";
string ByteCodes::mnem[lreturn] = "lreturn";
string ByteCodes::mnem[freturn] = "freturn";
string ByteCodes::mnem[dreturn] = "dreturn";
string ByteCodes::mnem[areturn] = "areturn";
string ByteCodes::mnem[return_] = "return_";
string ByteCodes::mnem[getstatic] = "getstatic";
string ByteCodes::mnem[putstatic] = "putstatic";
string ByteCodes::mnem[getfield] = "getfield";
string ByteCodes::mnem[putfield] = "putfield";
string ByteCodes::mnem[invokevirtual] = "invokevirtual";
string ByteCodes::mnem[invokespecial] = "invokespecial";
string ByteCodes::mnem[invokestatic] = "invokestatic";
string ByteCodes::mnem[invokeinterface] = "invokeinterface";
string ByteCodes::mnem[invokedynamic] = "invokedynamic";
string ByteCodes::mnem[new_] = "new_";
string ByteCodes::mnem[newarray] = "newarray";
string ByteCodes::mnem[anewarray] = "anewarray";
string ByteCodes::mnem[arraylength] = "arraylength";
string ByteCodes::mnem[athrow] = "athrow";
string ByteCodes::mnem[checkcast] = "checkcast";
string ByteCodes::mnem[instanceof_] = "instanceof_";
string ByteCodes::mnem[monitorenter] = "monitorenter";
string ByteCodes::mnem[monitorexit] = "monitorexit";
string ByteCodes::mnem[wide] = "wide";
string ByteCodes::mnem[multianewarray] = "multianewarray";
string ByteCodes::mnem[if_acmp_null] = "if_acmp_null";
string ByteCodes::mnem[if_acmp_nonnull] = "if_acmp_nonnull";
string ByteCodes::mnem[goto_w] = "goto_w";
string ByteCodes::mnem[jsr_w] = "jsr_w";
string ByteCodes::mnem[breakpoint] = "breakpoint";




