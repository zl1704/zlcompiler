/*
 * Pretty.cpp
 *
 *  Created on: 2017年12月27日
 *      Author: ZL
 */

#include "Pretty.hpp"
int* Pretty::prettyKey = new int;
Pretty::Pretty() {
	width = 4;
	lmargin = 0;
	prec = -1;
}

Pretty::~Pretty() {
}

Pretty* Pretty::instance() {
	Pretty* pretty = (Pretty*) Context::get(prettyKey);
	if (pretty == NULL) {
		pretty = new Pretty();
		Context::put(prettyKey, pretty);
	}
	return pretty;
}
void Pretty::debug(string msg,Tree* tree,int margin){
	if (util::debug) {
		for (int i = 0; i < margin; i++)
					cout << " ";
		cout << msg;
		tree->accept(instance());
		cout << endl;


	}

}
void Pretty::debug(string msg,Tree* tree){
	debug(msg,tree,0);
}


/**
 * 打印函数实现
 */

/**
 *   print stat
 */
void Pretty::printStat(Tree* tree) {
	printExpr(tree, TreeInfo::notExpression);
}
template<class T>
void Pretty::printStats(vector<T> trees) {
	for (int i = 0; i < trees.size(); ++i) {
		align();
		printStat(trees[i]);
		println();
	}
}

void Pretty::printExpr(Tree* tree) {
	printExpr(tree, TreeInfo::noPrec);
}
void Pretty::printExpr(Tree* tree, int prec) {
	int prevPrec = this->prec;
	try {
		this->prec = prec;
		if (tree == NULL)
			print("/*missing*/");
		else {
			tree->accept(this);
		}

	} catch (exception& e) {
		this->prec = prevPrec;
		cout << "visit Expr exception: " << e.what() << endl;
	}
	this->prec = prevPrec;
}

template<class T>
void Pretty::printExprs(vector<T> trees, string sep) {
	if (trees.size() > 0) {
		printExpr(trees[0]);
		for (int i = 1; i < trees.size(); i++) {
			print(sep);
			printExpr(trees[i]);
		}
	}
}

template<class T>
void Pretty::printExprs(vector<T> trees) {
	printExprs(trees, ",");
}

template<class T>
void Pretty::printBlock(vector<T> stats) {
	print("{");
	println();
	indent();
	printStats(stats);
	undent();
	align();
	print("}");
}

void Pretty::printBrackets(ArrayTypeTree* tree) {
	Tree* elem;
	while (true) {
		elem = tree->elemtype;
		print("[]");
		if (elem->getTag() != Tree::TYPEARRAY)
			break;
		tree = (ArrayTypeTree*) elem;
	}
}

void Pretty::printFlags(long long flags) {
	if ((flags & Flags::SYNTHETIC) != 0)
		print("/*synthetic*/ ");
	print(TreeInfo::flagNames(flags));
	if ((flags & Flags::StandardFlags) != 0)
		print(" ");
}
void Pretty::printBaseElementType(Tree* tree) {
	printExpr(innermostType(tree));
}
Tree* Pretty::innermostType(Tree* tree) {
	switch (tree->getTag()) {
	case Tree::TYPEARRAY:
		return innermostType(((ArrayTypeTree*) tree)->elemtype);
//	         case Tree::WILDCARD:
//	             return innermostType(((Wildcard*)tree)->inner);
	default:
		return tree;
	}
}

void Pretty::printUnit(CompilationUnit* tree, ClassDecl* cdef) {
	cout<<endl;
	cout<<"    [语法树]"<<endl;
	vector<Tree*> defs = tree->defs;
	for (int i = 0; i < defs.size(); i++) {
		printStat(defs[i]);
	}

}

/**
 * 访问方法实现
 */
void Pretty::visitTopLevel(CompilationUnit* tree) {
	try {
		printUnit(tree, NULL);

	} catch (exception& e) {
		cout << "visit CompilationUnit exception: " << e.what() << endl;
	}

}
void Pretty::visitClassDef(ClassDecl* tree) {
	try {
		println();
		align();
		enclClassName = tree->name;
		printFlags(tree->mods->flags & ~Flags::INTERFACE);
		print("class " + tree->name);
		print(" ");
		printBlock(tree->defs);
	} catch (exception& e) {
		cout << "visit ClassDef exception: " << e.what() << endl;
	}

}
void Pretty::visitMethodDef(MethodDecl* tree) {

	try {
		println();
		align();
		printExpr(tree->mods);

		//构造方法
		if (tree->name == Name::init) {
			if (enclClassName != "")
				print(enclClassName);
			else
				print(tree->name);
		}else {
			//返回类型
			printExpr(tree->restype);
			//方法名称
			print(" "+tree->name);
		}
		//参数
		print("(");
		printExprs(tree->params);
		print(")");
		//方法体
		if (tree->body != NULL) {
			print(" ");
			printStat(tree->body);
		} else {
			print(";");
		}

	} catch (exception& e) {
		cout << "visit MethodDef exception: " << e.what() << endl;
	}
}
void Pretty::visitVarDef(VariableDecl* tree) {
	try {
		//修饰符
		printExpr(tree->mods);
		//类型
		if ((tree->mods->flags & Flags::VARARGS) != 0) {
			printExpr(((ArrayTypeTree*) tree->vartype)->elemtype);
			print("... " + tree->name);
		} else {
			printExpr(tree->vartype);
			print(" " + tree->name);
		}
		//初始化
		if (tree->init != NULL) {
			print(" = ");
			printExpr(tree->init);
		}
		if (prec == TreeInfo::notExpression)
			print(";");
	} catch (exception& e) {
		cout << "visit VarDef exception: " << e.what() << endl;
	}
}

void Pretty::visitSkip(Skip* tree) {
	try {
		print(";");
	} catch (exception& e) {
		cout << "visit Skip exception: " << e.what() << endl;
	}
}
void Pretty::visitBlock(Block* tree) {
	try {
		printFlags(tree->flags);
		printBlock(tree->stats);
	} catch (exception& e) {
		cout << "visit Block exception: " << e.what() << endl;
	}
}
void Pretty::visitDoLoop(DoWhileLoop* tree) {
	try {
		print("do ");
		printStat(tree->body);
		align();
		print(" while ");
		if (tree->cond->getTag() == Tree::PARENS) {
			printExpr(tree->cond);
		} else {
			print("(");
			printExpr(tree->cond);
			print(")");
		}
		print(";");
	} catch (exception& e) {
		cout << "visit DoLoop exception: " << e.what() << endl;
	}
}
void Pretty::visitWhileLoop(WhileLoop* tree) {
	try {
		print("while ");
		if (tree->cond->getTag() == Tree::PARENS) {
			printExpr(tree->cond);
		} else {
			print("(");
			printExpr(tree->cond);
			print(")");
		}
		print(" ");
		printStat(tree->body);
	} catch (exception& e) {
		cout << "visit WhileLoop exception: " << e.what() << endl;
	}
}
void Pretty::visitForLoop(ForLoop* tree) {
	try {
		print("for (");
		if (tree->init.size() > 0) {
			if (tree->init[0]->getTag() == Tree::VARDEF) {
				printExpr(tree->init[0]);
				for (int i = 1; i < tree->init.size(); i++) {
					VariableDecl* vdef = (VariableDecl*) tree->init[i];
					print(", " + vdef->name + " = ");
					printExpr(vdef->init);
				}
			} else {
				printExprs(tree->init);
			}
		}
		print("; ");
		if (tree->cond != NULL)
			printExpr(tree->cond);
		print("; ");
		printExprs(tree->step);
		print(") ");
		printStat(tree->body);
	} catch (exception& e) {
		cout << "visit ForLoop exception: " << e.what() << endl;
	}
}

void Pretty::visitSwitch(Switch* tree) {
	try {
		print("switch ");
		if (tree->selector->getTag() == Tree::PARENS) {
			printExpr(tree->selector);
		} else {
			print("(");
			printExpr(tree->selector);
			print(")");
		}
		print(" {");
		println();
		printStats(tree->cases);
		align();
		print("}");
	} catch (exception& e) {
		cout << "visit Switch exception: " << e.what() << endl;
	}
}
void Pretty::visitCase(Case* tree) {
	try {
		if (tree->pat == NULL) {
			print("default");
		} else {
			print("case ");
			printExpr(tree->pat);
		}
		print(": ");
		println();
		indent();
		printStats(tree->stats);
		undent();
		align();
	} catch (exception& e) {
		cout << "visit Case exception: " << e.what() << endl;
	}

}

void Pretty::visitConditional(Conditional* tree) {
	try {
		open(prec, TreeInfo::condPrec);
		printExpr(tree->cond, TreeInfo::condPrec);
		print(" ? ");
		printExpr(tree->truepart, TreeInfo::condPrec);
		print(" : ");
		printExpr(tree->falsepart, TreeInfo::condPrec);
		close(prec, TreeInfo::condPrec);
	} catch (exception& e) {
		cout << "visit Conditional exception: " << e.what() << endl;
	}
}
void Pretty::visitIf(If* tree) {
	try {
		print("if ");
		if (tree->cond->getTag() == Tree::PARENS) {
			printExpr(tree->cond);
		} else {
			print("(");
			printExpr(tree->cond);
			print(")");
		}
		print(" ");
		printStat(tree->thenpart);
		if (tree->elsepart != NULL) {
			print(" else ");
			printStat(tree->elsepart);
		}
	} catch (exception& e) {
		cout << "visit If exception: " << e.what() << endl;
	}
}
void Pretty::visitExec(ExpressionStatement* tree) {
	try {
		printExpr(tree->expr);
		if (prec == TreeInfo::notExpression)
			print(";");
	} catch (exception& e) {
		cout << "visit Exec exception: " << e.what() << endl;
	}
}
void Pretty::visitBreak(Break* tree) {
	try {
		print("break");
		if (tree->label != "")
			print(" " + tree->label);
		print(";");
	} catch (exception& e) {
		cout << "visit Break exception: " << e.what() << endl;
	}
}
void Pretty::visitContinue(Continue* tree) {
	try {
		print("continue");
		if (tree->label != "")
			print(" " + tree->label);
		print(";");

	} catch (exception& e) {
		cout << "visit Continue exception: " << e.what() << endl;
	}
}
void Pretty::visitReturn(Return* tree) {
	try {
		print("return");
		if (tree->expr != NULL) {
			print(" ");
			printExpr(tree->expr);
		}
		print(";");
	} catch (exception& e) {
		cout << "visit Return exception: " << e.what() << endl;
	}
}
void Pretty::visitApply(MethodInvocation* tree) {
	try {
		tree->getTag();
		printExpr(tree->meth);
		print("(");
		printExprs(tree->args);
		print(")");

	} catch (exception& e) {
		cout << "visit Apply exception: " << e.what() << endl;
	}
}
void Pretty::visitNewClass(NewClass* tree) {
	try {
		if (tree->encl != NULL) {
			printExpr(tree->encl);
			print(".");
		}
		print("new ");
		printExpr(tree->clazz);
		print("(");
		printExprs(tree->args);
		print(")");
		if (tree->def != NULL) {
			string enclClassNamePrev = enclClassName;
//		                enclClassName =
//		                        tree->def->name != NULL ? tree->def->name :
//		                            tree->type != NULL && tree->type->tsym->name != tree->type->tsym->name->table->names->empty
//		                                ? tree.type.tsym.name : null;
			if ((tree->def->mods->flags & Flags::ENUM) != 0)
				print("/*enum*/");
			printBlock(tree->def->defs);
			enclClassName = enclClassNamePrev;
		}
	} catch (exception& e) {
		cout << "visit NewClass exception: " << e.what() << endl;
	}
}
void Pretty::visitNewArray(NewArray* tree) {
	try {
		if (tree->elemtype != NULL) {
			print("new ");
			Tree* elem = tree->elemtype;
			if (elem->getTag() == Tree::TYPEARRAY)
				printBaseElementType((ArrayTypeTree*) elem);
			else
				printExpr(elem);

			for (int i = 0; i < tree->dims.size(); i++) {
				print("[");
				printExpr(tree->dims[i]);
				print("]");
			}
			if (dynamic_cast<ArrayTypeTree *>(elem))
				printBrackets((ArrayTypeTree*) elem);
		}
		if (tree->elems.size() != 0) {
			if (tree->elemtype != NULL)
				print("[]");
			print("{");
			printExprs(tree->elems);
			print("}");
		}
	} catch (exception& e) {
		cout << "visit NewArray exception: " << e.what() << endl;
	}

}

void Pretty::visitParens(Parens* tree) {
	try {
		print("(");
		printExpr(tree->expr);
		print(")");
	} catch (exception& e) {
		cout << "visit Parens exception: " << e.what() << endl;
	}
}
void Pretty::visitAssign(Assign* tree) {
	try {
		open(prec, TreeInfo::assignPrec);
		printExpr(tree->lhs, TreeInfo::assignPrec + 1);
		print(" = ");
		printExpr(tree->rhs, TreeInfo::assignPrec);
		close(prec, TreeInfo::assignPrec);
	} catch (exception& e) {
		cout << "visit Assign exception: " << e.what() << endl;
	}
}
string operatorName(int tag) {
	switch (tag) {
	case Tree::POS:
		return "+";
	case Tree::NEG:
		return "-";
	case Tree::NOT:
		return "!";
	case Tree::COMPL:
		return "~";
	case Tree::PREINC:
		return "++";
	case Tree::PREDEC:
		return "--";
	case Tree::POSTINC:
		return "++";
	case Tree::POSTDEC:
		return "--";
	case Tree::NULLCHK:
		return "<*nullchk*>";
	case Tree::OR:
		return "||";
	case Tree::AND:
		return "&&";
	case Tree::EQ:
		return "==";
	case Tree::NE:
		return "!=";
	case Tree::LT:
		return "<";
	case Tree::GT:
		return ">";
	case Tree::LE:
		return "<=";
	case Tree::GE:
		return ">=";
	case Tree::BITOR:
		return "|";
	case Tree::BITXOR:
		return "^";
	case Tree::BITAND:
		return "&";
	case Tree::SL:
		return "<<";
	case Tree::SR:
		return ">>";
	case Tree::USR:
		return ">>>";
	case Tree::PLUS:
		return "+";
	case Tree::MINUS:
		return "-";
	case Tree::MUL:
		return "*";
	case Tree::DIV:
		return "/";
	case Tree::MOD:
		return "%";
	default:
		throw "操作符错误";
	}
}

void Pretty::visitAssignop(AssignOp* tree) {
	try {
		open(prec, TreeInfo::assignopPrec);
		printExpr(tree->lhs, TreeInfo::assignopPrec + 1);
		print(" " + operatorName(tree->getTag() - Tree::ASGOffset) + "= ");
		printExpr(tree->rhs, TreeInfo::assignopPrec);
		close(prec, TreeInfo::assignopPrec);
	} catch (exception& e) {
		cout << "visit Assignop exception: " << e.what() << endl;
	}
}
void Pretty::visitUnary(Unary* tree) {

	try {
		int ownprec = TreeInfo::opPrec(tree->getTag());
		string opname = operatorName(tree->getTag());
		open(prec, ownprec);
		if (tree->getTag() <= Tree::PREDEC) {
			print(opname);
			printExpr(tree->arg, ownprec);
		} else {
			printExpr(tree->arg, ownprec);
			print(opname);
		}
		close(prec, ownprec);
	} catch (exception& e) {
		cout << "visit Unary exception: " << e.what() << endl;
	}
}
void Pretty::visitBinary(Binary* tree) {
	try {
		if(tree->type->isConst()){
			printConst(tree->getTag(),((ConstType*)tree->type)->str);
		}else{
			int ownprec = TreeInfo::opPrec(tree->getTag());
			string opname = operatorName(tree->getTag());
			open(prec, ownprec);
			printExpr(tree->lhs, ownprec);
			print(" " + opname + " ");
			printExpr(tree->rhs, ownprec + 1);
			close(prec, ownprec);
		}
	} catch (exception& e) {
		cout << "visit Binary exception: " << e.what() << endl;
	}
}
void Pretty::visitTypeCast(TypeCast* tree) {
	try {
		open(prec, TreeInfo::prefixPrec);
		print("(");
		printExpr(tree->clazz);
		print(")");
		printExpr(tree->expr, TreeInfo::prefixPrec);
		close(prec, TreeInfo::prefixPrec);
	} catch (exception& e) {
		cout << "visit TypeCast exception: " << e.what() << endl;
	}
}
void Pretty::visitTypeTest(InstanceOf* tree) {
	try {
		open(prec, TreeInfo::ordPrec);
		printExpr(tree->expr, TreeInfo::ordPrec);
		print(" instanceof ");
		printExpr(tree->clazz, TreeInfo::ordPrec + 1);
		close(prec, TreeInfo::ordPrec);
	} catch (exception& e) {
		cout << "visit TypeTest exception: " << e.what() << endl;
	}
}
void Pretty::visitIndexed(ArrayAccess* tree) {
	try {
		printExpr(tree->indexed, TreeInfo::postfixPrec);
		print("[");
		printExpr(tree->index);
		print("]");
	} catch (exception& e) {
		cout << "visit Indexed exception: " << e.what() << endl;
	}
}
void Pretty::visitSelect(FieldAccess* tree) {

	try {
		printExpr(tree->selected, TreeInfo::postfixPrec);
		print("." + tree->name);
	} catch (exception& e) {
		cout << "visit Select exception: " << e.what() << endl;
	}
}
void Pretty::visitIdent(Ident* tree) {
	try {
		print(tree->name);
	} catch (exception& e) {
		cout << "visit Ident exception: " << e.what() << endl;
	}
}
void Pretty::printConst(int type,string val){
	switch (type) {
			case TypeTags::INT:
				print(val);
				break;
			case TypeTags::LONG:
				print(val + "L");
				break;
			case TypeTags::FLOAT:
				print(val + "F");
				break;
			case TypeTags::DOUBLE:
				print(val);
				break;
			case TypeTags::CHAR:
				print("\'" + val + "\'");
				break;
			case TypeTags::BOOLEAN:
				print(val=="1"?"true":"false");
				break;
			case TypeTags::BOT:
				print("null");
				break;
			default:
				print("\"" + val + "\"");
				break;
			}
}
void Pretty::visitLiteral(Literal* tree) {
	try {

		printConst(tree->typetag,tree->value);
	} catch (exception& e) {
		cout << "visit Literal exception: " << e.what() << endl;
	}
}
void Pretty::visitTypeIdent(PrimitiveTypeTree* tree) {
	try {
		switch (tree->typetag) {
		case TypeTags::BYTE:
			print("byte");
			break;
		case TypeTags::CHAR:
			print("char");
			break;
		case TypeTags::SHORT:
			print("short");
			break;
		case TypeTags::INT:
			print("int");
			break;
		case TypeTags::LONG:
			print("long");
			break;
		case TypeTags::FLOAT:
			print("float");
			break;
		case TypeTags::DOUBLE:
			print("double");
			break;
		case TypeTags::BOOLEAN:
			print("boolean");
			break;
		case TypeTags::VOID:
			print("void");
			break;
		case TypeTags::STRING:
			print("String");
			break;
		default:
			print("error");
			break;
		}
	} catch (exception& e) {
		cout << "visit TypeIdent exception: " << e.what() << endl;
	}
}
void Pretty::visitTypeArray(ArrayTypeTree* tree) {
	try {
		printBaseElementType(tree);
		printBrackets(tree);
	} catch (exception& e) {
		cout << "visit TypeArray exception: " << e.what() << endl;
	}

}
void Pretty::visitModifiers(Modifiers* tree) {
	try {
		printFlags(tree->flags);
	} catch (exception& e) {
		cout << "visit Modifiers exception: " << e.what() << endl;
	}

}
void Pretty::visitTree(Tree* tree) {
	try {
		print("(UNKNOWN:  tree  )");
		println();
	} catch (exception& e) {
		cout << "visit Tree exception: " << e.what() << endl;
	}

}
