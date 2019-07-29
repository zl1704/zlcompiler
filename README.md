# zlcompiler
zlcompiler一个类JAVA语法的编译器,学习编译原理的实践，为以后研究编译器后端做基础
## 功能
* 词法分析
* 语法分析
* 语义分析

* 代码生成(进行中)

	生成JAVA字节码，能在JVM上运行，相关代码在jvm包下
	
	* ByteCodes ：Java字节码 
	* Item : 生成代码过程中的数据结构 
	* Code : 代码生成核心类，每个method对应一个Code 
	* Gen  : 代码生成流程控制类，控制着代码生成的流程 

### Code


+ addLocalVar(VarSymbol* v) 

	添加一个局部变量到表中 
+ void setDefined(int adr) 

	标记adr寄存器 
+ void endScopes(int adr) addLineNumber(int startPc,int lineNumber) statBegin(int pos)	   

	从局部变量表中删除adr之后寄存器中的变量 
+ markStatBegin()
 		 
	行号与字节码映射，用于Debug调试 
+ emit相关方法：

	输出字节码指令

### Gen

+ genClass
	
	代码生成的入口，以Class为生成单元，将(静态)成员变量初始化和(静态)构造块中代码添加到(静态)构造方法中
+ genMethod

	进入方法，生成code

+ genLoop

	循环的翻译，有两种循环，whlie(for循环:init+while), do-while
	
	1. while:
	
			1. 生成cond语句
			
			2. 生成false跳转语句
			
			3. 回填true跳转
			
			4. 生成body语句
			
			5. 生成跳转并回填跳转到循环开始,continue回填
			
			6. 回填循环出口(false跳转)，break回填
			
			
	
	2. do-while与while形式类似，不同的地方在于生成cond的地方在body之后，然后并回填true,false跳转链
	
	3. 方法调用语句
		
		```
		
		class A{

			public void fun(double a, double b){
				fun2(1,2);
				int c = (a>b?false:true) ? fun(1,2):2;
			
			}
		
			public int fun2(int a,int b){
				return a+b;
			
			}

		}
		
		```
	
	
		1.	Gen::visitExec:	fun(1,2);
		生成指令：
		
		```
		
		Gen :	 aload_0 //压入this
		Gen :	 iconst_1
		Gen :	 iconst_2
		Gen :	 invokespecial
		Gen :	 pop    //因为fun2有返回值，Exec之后需要将返回值从栈中弹出
		
		```
		
		JAVA中参数入栈由调用者维护，出栈由JVM维护， 实现方式为push return_address + jmp target（call）,  pop + jmp return_address(在_return指令中完成)
		
		2.  
		
		```
		
		Gen::visitWhileLoop:
			while (a < b) {
				a = a + 1;
				a += 1;
			}
		
		//生成代码
		
		
		Gen :	 dload_1
		Gen :	 dload_3
		Gen :	 dcmpg
		Gen :	 ifge	0
		
		//a = a+1;
		Gen :	 dload_1
		Gen :	 dconst_1
		Gen :	 dadd
		Gen :	 dstore_1
		
		//a +=1;
		Gen :	 dload_1
		Gen :	 dload_1
		Gen :	 dconst_1
		Gen :	 dadd
		Gen :	 dstore_1
		Gen :	 goto_	0
		Gen :	 return_

		
	

