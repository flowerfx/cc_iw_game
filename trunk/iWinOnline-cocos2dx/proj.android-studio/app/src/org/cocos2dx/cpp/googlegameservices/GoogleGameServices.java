package org.cocos2dx.cpp.googlegameservices;

import org.cocos2dx.cpp.googlegameservices.GoogleGameServicesManager;

/*
 * ThinhTrinh
 * */

public class GoogleGameServices {
	public static final String TAG = "GameCenterService";
	private static GoogleGameServicesManager manager = null;

	/**
	 * true: Support login with GoogleGameServices
	 * */
	public static boolean isSupportLoginGoogleGameServices() {
		boolean isIOS = false;
		return isIOS;
	}

	/** @return null if class is not available in runtime */
	private static Class<?> findClass(String name) {
		try {
			return Class.forName(name);
		} catch (Exception e) {
			e.printStackTrace();
			return null;
		}
	}

	/** We try to locate a suitable store via Java reflection. */
	private static void resolve() {
		try {
			// check if we are on Android
			boolean android = findClass("org.cocos2dx.cpp.AppActivity") != null;
			if (android) {
				try {
					Class<?> iapClazz = Class.forName("org.cocos2dx.cpp.googlegameservices.AndroidGGSManager");
					GoogleGameServices.manager = (GoogleGameServicesManager) iapClazz.newInstance();
				} catch (Exception e) {
					// some jar files appear to be missing
					e.printStackTrace();
				}
				return;
			}
			// check if we are on iOS
//			boolean ios = findClass("com.badlogic.gdx.backends.iosrobovm.IOSApplication") != null;
//			if (ios) {
//				return;
//			}
		} catch (Exception e) {
			// we appear not to be on libGDX!
			e.printStackTrace();
		}
	}

	/** Returns the registered manager or null for none. */
	public static GoogleGameServicesManager getManager() {
		// resolve our manager via reflection if we do not have one
		if (manager == null) {
			resolve();
		}
		
		// return the manager or null if none was found
		return manager;
	}

	/** Returns true if there is a purchase manager available. */
	public static boolean hasManager() {
		return getManager() != null;
	}

}
