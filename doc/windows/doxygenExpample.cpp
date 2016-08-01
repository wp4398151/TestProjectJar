/**
 * @file
 * @brief 该文档是用来给出doxygen的例子的
 *
 * @mainpage http://www.lenovo.com
 *
 * @section 只有一部分
 *
 * @author Wupeng <mailto:4398151@qq.com>
 *
 * 给出了例子。as [The link text](@ref Afterdoc_Test) 
 *
 */
 
 /**
  * A test class 
  */
 class Afterdoc_Test{  
	public:    
	/** An enum type.      
	*  The documentation block cannot be put after the enum!      
	*/    
	enum EnumType    
	{      
		int EVal1,     /**< enum value 1 */      
		int EVal2      /**< enum value 2 */    
	};    
	void member();   /**< a member function.      */
	
	/**       
	 * a normal member taking two arguments and returning an integer value.       
	 * @param a an integer argument.       
	 * @param s a constant character pointer.       
	 * @see Javadoc_Test()       
	 * @see ~Javadoc_Test()       
	 * @see testMeToo()       
	 * @see publicVar()       
	 * @return The test results       
	 */       
	 int testMe(int a,const char *s);             
	 /**       
	  * A pure virtual member.       
	  * @see testMe()       
	  * @param c1 the first argument.       
	  * @param c2 the second argument.       
	  */       
	  virtual void testMeToo(char c1,char c2) = 0;
protected:    
	int value;       /**< an integer value */
};