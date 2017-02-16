/****************************************************************************
Copyright (c) 2015 Chukong Technologies Inc.
 
http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/
package org.cocos2dx.cpp;

import android.Manifest;
import android.app.Dialog;
import android.content.ContextWrapper;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.annotation.NonNull;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;
import android.view.View;

import org.cocos2dx.cpp.googlegameservices.GGSManager;
import org.cocos2dx.cpp.googlegameservices.GameHelper;
import org.cocos2dx.cpp.googlegameservices.IActionListener;
import org.cocos2dx.cpp.googlegameservices.MGoogleAccount;
import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;

import com.google.android.gms.auth.api.Auth;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInResult;
import com.sdkbox.plugin.SDKBox;

import iwin.vn.full.R;


public class AppActivity extends Cocos2dxActivity{

    private static final String TAG = "AppActivity";

    public static AppActivity instance;

    private GameHelper gameHelper = null;
    private GameHelper.GameHelperListener gameHelperListener;

    public static Dialog loader_dialog;
    private Cocos2dxGLSurfaceView glSurfaceView;

    private int mSystemUiVisibility =  View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION // hide nav bar
            | View.SYSTEM_UI_FLAG_FULLSCREEN // hide status bar
            | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY;

    private static final int MY_PERMISSIONS_REQUEST_GET_ACCOUNTS = 123;

    @Override
    protected void onCreate(final Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        SDKBox.init(this);
        AndroidPlatform.setActivity(this);
        IwinIapHelper.init(this);

        AppActivity.instance = this;

        loader_dialog = new Dialog(this,R.style.Loader);
        loader_dialog.setContentView(R.layout.loader);
        if (android.os.Build.VERSION.SDK_INT >= 11) {
            loader_dialog.getWindow().getDecorView().setSystemUiVisibility(mSystemUiVisibility);
        }
        loader_dialog.show();

        if (gameHelper == null) {
            gameHelper = new GameHelper(this, GameHelper.CLIENT_GAMES);
            gameHelper.enableDebugLog(true);
            gameHelperListener = new GameHelper.GameHelperListener() {

                @Override
                public void onSignInSucceeded() {
                    try {
                        gameHelper.getToken(new IActionListener() {

                            @Override
                            public void actionPerformed(final Object o) {
                                if (o != null) {
                                    runOnUiThread(new Runnable() {
                                        @Override
                                        public void run() {
                                            GGSManager.instanse.listener.loginSuccessed((String) o);
                                        }
                                    });
                                }
                            }
                        });
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }

                @Override
                public void onSignInFailed() {

                }
            };
            gameHelper.setup(gameHelperListener);
            gameHelper.setConnectOnStart(false);
        }
    }

    public static void dismissLoader() {
        if (loader_dialog != null) {
            loader_dialog.dismiss();
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data){
        if(!SDKBox.onActivityResult(requestCode, resultCode, data)  && !IwinIapHelper.onActivityResult(requestCode, resultCode, data))
        {
            super.onActivityResult(requestCode, resultCode, data);
        }
        gameHelper.onActivityResult(requestCode, resultCode, data);
    }

    @Override
    protected void onStart(){
        super.onStart();
        SDKBox.onStart();
    }

    @Override
    protected void onStop(){
        super.onStop();
        SDKBox.onStop();
    }

    @Override
    protected void onResume(){
        super.onResume();
        SDKBox.onResume();
    }

    @Override
    protected void onPause(){
        super.onPause();
        SDKBox.onPause();
    }

    @Override
    public void onDestroy(){
        super.onDestroy();
        IwinIapHelper.onDestroy();
    }

    @Override
    public void onBackPressed(){
        if(!SDKBox.onBackPressed()){
            super.onBackPressed();
        }

    }

    public void logInGGS() {
        try {
            // Here, thisActivity is the current activity
            if (checkSelfPermission(Manifest.permission.GET_ACCOUNTS)
                    != PackageManager.PERMISSION_GRANTED) {
                // Should we show an explanation?
                if (shouldShowRequestPermissionRationale(Manifest.permission.GET_ACCOUNTS)) {
                    // Show an expanation to the user *asynchronously* -- don't block
                    // this thread waiting for the user's response! After the user
                    // sees the explanation, try again to request the permission.
                } else {
                    // No explanation needed, we can request the permission.
                    requestPermissions(new String[]{Manifest.permission.GET_ACCOUNTS},
                            MY_PERMISSIONS_REQUEST_GET_ACCOUNTS);
                    // MY_PERMISSIONS_REQUEST_READ_CONTACTS is an
                    // app-defined int constant. The callback method gets the
                    // result of the request.
                }
            }
            else {
                runOnUiThread(new Runnable() {
                    @Override
                    public void run() {
                        gameHelper.beginUserInitiatedSignIn();
                    }
                });
            }
        } catch (Exception e) {
            Log.e(TAG, "checkSelfPermission error " + e);
        }
    }

    //@Override
    public void onRequestPermissionsResult(int requestCode,
                                           String permissions[], int[] grantResults) {
        switch (requestCode) {
            case MY_PERMISSIONS_REQUEST_GET_ACCOUNTS: {
                // If request is cancelled, the result arrays are empty.
                if (grantResults.length > 0
                        && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // permission was granted, yay! Do the
                    // contacts-related task you need to do.
                    runOnUiThread(new Runnable() {
                        @Override
                        public void run() {
                            gameHelper.beginUserInitiatedSignIn();
                        }
                    });
                } else {

                    // permission denied, boo! Disable the
                    // functionality that depends on this permission.
                }
                return;
            }

            // other 'case' lines to check for other
            // permissions this app might request
        }
    }

    public void logOutGGS() {
        try {
            runOnUiThread(new Runnable() {

                @Override
                public void run() {
                gameHelper.signOut();
                GGSManager.instanse.listener.logOutSuccessed();
                }
            });
        } catch (Exception e) {

        }
    }

    public boolean isLogiInGGS() {
        return gameHelper.isSignedIn();
    }

    public MGoogleAccount getCurrentGoogleAccount() {
        return gameHelper.getProfileInformation();
    }

    public void inItGGS() {
        gameHelper.setConnectOnStart(true);
    }
}
