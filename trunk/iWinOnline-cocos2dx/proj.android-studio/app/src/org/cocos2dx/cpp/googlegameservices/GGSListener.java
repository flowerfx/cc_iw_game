package org.cocos2dx.cpp.googlegameservices;

public interface GGSListener {
	
	public void loginSuccessed(String token);

	public void loginFailed();
	
	public void logOutSuccessed();

	public void logOutFailed();
	    
}
