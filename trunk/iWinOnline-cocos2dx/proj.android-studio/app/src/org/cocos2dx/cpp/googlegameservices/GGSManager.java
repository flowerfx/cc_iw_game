package org.cocos2dx.cpp.googlegameservices;

import org.cocos2dx.cpp.AppActivity;

public class GGSManager {
	public final GGSListener listener;
	public static GGSManager instanse;
	public GGSManager(GGSListener ggsListener){
		GGSManager.instanse = this;
		this.listener = ggsListener;
		AppActivity.instance.inItGGS();
		
	}
	
	public void logIn(){
		AppActivity.instance.logInGGS();
		
	}
	
	public void logOut(){
		AppActivity.instance.logOutGGS();
		
	}
}
