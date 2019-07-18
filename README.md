# zlcompiler
zlcompiler一个类JAVA语法的编译器,学习编译原理的实践，为以后研究编译器后端做基础
## 功能
* 词法分析
* 语法分析
* 语义分析
	** 
* 代码生成(进行中)
	生成JAVA字节码，能在JVM上运行，相关代码在jvm包下
	** ByteCodes ：Java字节码
	** Item : 生成代码过程中的数据结构
	** Code : 代码生成核心类，每个方法对应一个Code
		方法|功能
		--|:--:
		addLocalVar(VarSymbol* v) | 添加一个局部变量到表中
		void setDefined(int adr)  | 标记adr寄存器
		void endScopes(int adr)   | 从局部变量表中删除adr之后寄存器中的变量
		addLineNumber(int startPc,int lineNumber)
		statBegin(int pos)		
		markStatBegin() 		  | 行号与字节码映射，用于Debug调试

		
	** Gen  : 代码生成流程控制类，控制着代码生成的流程
	
	

