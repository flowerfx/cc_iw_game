package org.cocos2dx.cpp.googlegameservices;

import org.cocos2dx.cpp.googlegameservices.MGoogleAccount;
import org.cocos2dx.cpp.googlegameservices.IActionListener;

/*
 * ThinhTrinh
 * */

public interface GoogleGameServicesManager {
	
	public String getAccessToken();
	
	public void setAccessToken(String accessToken);
	
	public void login(IActionListener callback);
	
	public void login();
	
	public void logOut(IActionListener callback);
	
	public void rateGame();
	
	public void submitScore(long score);
	
	public void showScores();
	
	public boolean isSignedIn();
	
	public MGoogleAccount getCurrentAccount();
	
	public void setCurrentAccount(MGoogleAccount googleAccount);
}
