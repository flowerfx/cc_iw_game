package org.cocos2dx.cpp.googlegameservices;


import android.util.Log;

import org.cocos2dx.cpp.googlegameservices.MGoogleAccount;
import org.cocos2dx.cpp.googlegameservices.GoogleGameServicesManager;
import org.cocos2dx.cpp.googlegameservices.IActionListener;
import org.cocos2dx.cpp.AppActivity;

public class AndroidGGSManager implements GoogleGameServicesManager{
	private GGSManager gGSManager;

	public static final String TAG = "AndroidGGSManager";
	public String accessToken;
	public MGoogleAccount googleAccount;
	private IActionListener callback;

	@Override
	public void rateGame() {
		// TODO Auto-generated method stub

	}

	@Override
	public void submitScore(long score) {
		// TODO Auto-generated method stub

	}

	@Override
	public void showScores() {
		// TODO Auto-generated method stub

	}

	@Override
	public boolean isSignedIn() {
		return accessToken == null ? false: true;

	}

	public GGSManager getGGSManager(){
		if (gGSManager == null) {
			gGSManager = new GGSManager(new GGSListener() {

				@Override
				public void loginSuccessed(String token) {
					if (token != null && token != "") {
						callback.actionPerformed(token);
						//System.out.println("token "+ token);
						setCurrentAccount(AppActivity.instance.getCurrentGoogleAccount());
					}
				}

				@Override
				public void loginFailed() {
					// TODO Auto-generated method stub

				}

				@Override
				public void logOutSuccessed() {
					callback.actionPerformed(null);
				}

				@Override
				public void logOutFailed() {
					// TODO Auto-generated method stub

				}
			});
		}
		return gGSManager;
	}

	@Override
	public void setAccessToken(String accessToken) {
		this.accessToken = accessToken;
	}

	@Override
	public void login(final IActionListener callback) {
		this.callback = callback;
		getGGSManager().logIn();

	}

	@Override
	public void logOut(IActionListener callback) {
		setCurrentAccount(null);
		setAccessToken("");
		this.callback = callback;
		getGGSManager().logOut();

	}

	@Override
	public String getAccessToken() {
		return accessToken;

	}

	@Override
	public MGoogleAccount getCurrentAccount() {
		return this.googleAccount;

	}

	@Override
	public void setCurrentAccount(MGoogleAccount googleAccount) {
		this.googleAccount = googleAccount;

	}

	@Override
	public void login() {
		getGGSManager().logIn();
	}

}
