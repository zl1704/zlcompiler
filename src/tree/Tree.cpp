/*
 * Tree.cpp
 *
 *  Created on: 2017年12月23日
 *      Author: ZL
 */

#include "Tree.hpp"
int TreeInfo::opPrec(int op) {
	switch (op) {
	case Tree::POS:
	case Tree::NEG:
	case Tree::NOT:
	case Tree::COMPL:
	case Tree::PREINC:
	case Tree::PREDEC:
		return prefixPrec;
	case Tree::POSTINC:
	case Tree::POSTDEC:
	case Tree::NULLCHK:
		return postfixPrec;
	case Tree::ASSIGN:
		return assignPrec;
	case Tree::BITOR_ASG:
	case Tree::BITXOR_ASG:
	case Tree::BITAND_ASG:
	case Tree::SL_ASG:
	case Tree::SR_ASG:
	case Tree::USR_ASG:
	case Tree::PLUS_ASG:
	case Tree::MINUS_ASG:
	case Tree::MUL_ASG:
	case Tree::DIV_ASG:
	case Tree::MOD_ASG:
		return assignopPrec;
	case Tree::OR:
		return orPrec;
	case Tree::AND:
		return andPrec;
	case Tree::EQ:
	case Tree::NE:
		return eqPrec;
	case Tree::LT:
	case Tree::GT:
	case Tree::LE:
	case Tree::GE:
		return ordPrec;
	case Tree::BITOR:
		return bitorPrec;
	case Tree::BITXOR:
		return bitxorPrec;
	case Tree::BITAND:
		return bitandPrec;
	case Tree::SL:
	case Tree::SR:
	case Tree::USR:
		return shiftPrec;
	case Tree::PLUS:
	case Tree::MINUS:
		return addPrec;
	case Tree::MUL:
	case Tree::DIV:
	case Tree::MOD:
		return mulPrec;
	case Tree::TYPETEST:
		return ordPrec;
	default:
		return -1;
	}

}
string TreeInfo::flagNames(long long flags) {
	string buf = "";
	if ((flags & Flags::PUBLIC) != 0)
		buf += " public";
	if ((flags & Flags::PRIVATE) != 0)
		buf += " private";
	if ((flags & Flags::PROTECTED) != 0)
		buf += " protected";
	if ((flags & Flags::STATIC) != 0)
		buf += " static";
	if ((flags & Flags::FINAL) != 0)
		buf += " final ";
	if ((flags & Flags::SYNCHRONIZED) != 0)
		buf += " synchronized";
	if ((flags & Flags::VOLATILE) != 0)
		buf += " volatile ";
	if ((flags & Flags::TRANSIENT) != 0)
		buf += " transient";
	if ((flags & Flags::NATIVE) != 0)
		buf += " native";
	if ((flags & Flags::INTERFACE) != 0)
		buf += " interface";
	if ((flags & Flags::ABSTRACT) != 0)
		buf += " abstract";
	if ((flags & Flags::STRICTFP) != 0)
		buf += " strictfp";
	if ((flags & Flags::BRIDGE) != 0)
		buf += " bridge";
	if ((flags & Flags::SYNTHETIC) != 0)
		buf += " synthetic";
	if ((flags & Flags::DEPRECATED) != 0)
		buf += " deprecated";
	if ((flags & Flags::HASINIT) != 0)
		buf += " hasinit";
	if ((flags & Flags::ENUM) != 0)
		buf += " enum";
	if ((flags & Flags::IPROXY) != 0)
		buf += " iproxy";
	if ((flags & Flags::NOOUTERTHIS) != 0)
		buf += " noouterthis";
	if ((flags & Flags::EXISTS) != 0)
		buf += " exists";
	if ((flags & Flags::COMPOUND) != 0)
		buf += " compound";
	if ((flags & Flags::CLASS_SEEN) != 0)
		buf += " class_seen";
	if ((flags & Flags::SOURCE_SEEN) != 0)
		buf += " source_seen";
	if ((flags & Flags::LOCKED) != 0)
		buf += " locked";
	if ((flags & Flags::UNATTRIBUTED) != 0)
		buf += " unattributed";
	if ((flags & Flags::ANONCONSTR) != 0)
		buf += " anonconstr";
	if ((flags & Flags::ACYCLIC) != 0)
		buf += " acyclic";
	if ((flags & Flags::PARAMETER) != 0)
		buf += " parameter";
	if ((flags & Flags::VARARGS) != 0)
		buf += " varargs";
	return buf;

}

string TreeInfo::operatorName(int tag) {
	switch (tag - Tree::POS) {

	case (Tree::POS - Tree::POS):
		return "+";
	case (Tree::NEG - Tree::POS):
		return "-";
	case (Tree::NOT - Tree::POS):
		return "!";
	case (Tree::COMPL - Tree::POS):
		return "~";
	case (Tree::PREINC - Tree::POS):
		return "++";
	case (Tree::PREDEC - Tree::POS):
		return "--";
	case (Tree::POSTINC - Tree::POS):
		return "++";
	case (Tree::POSTDEC - Tree::POS):
		return "--";
	case (Tree::NULLCHK - Tree::POS):
		return "<*nullchk*>";
	case (Tree::OR - Tree::POS):
		return "||";
	case (Tree::AND - Tree::POS):
		return "&&";
	case (Tree::EQ - Tree::POS):
		return ": return: return";
	case (Tree::NE - Tree::POS):
		return "!: return";
	case (Tree::LT - Tree::POS):
		return "<";
	case (Tree::GT - Tree::POS):
		return ">";
	case (Tree::LE - Tree::POS):
		return "<: return";
	case (Tree::GE - Tree::POS):
		return ">: return";
	case (Tree::BITOR - Tree::POS):
		return "|";
	case (Tree::BITXOR - Tree::POS):
		return "^";
	case (Tree::BITAND - Tree::POS):
		return "&";
	case (Tree::SL - Tree::POS):
		return "<<";
	case (Tree::SR - Tree::POS):
		return ">>";
	case (Tree::USR - Tree::POS):
		return ">>>";
	case (Tree::PLUS - Tree::POS):
		return "+";
	case (Tree::MINUS - Tree::POS):
		return "-";
	case (Tree::MUL - Tree::POS):
		return "";
	case (Tree::DIV - Tree::POS):
		return "*";
	case (Tree::MOD - Tree::POS):
		return "%";
	default:
		return "error";

	}

}



