package org.cocos2dx.cpp;

import iwin.vn.full.R;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.lang.reflect.Array;
import java.util.ArrayList;

import org.cocos2dx.cpp.googlegameservices.GoogleGameServices;
import org.cocos2dx.cpp.googlegameservices.IActionListener;



import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.Dialog;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ClipboardManager;
import android.content.ClipData;
import android.content.pm.PackageManager;
import android.content.pm.PackageManager.NameNotFoundException;
import android.content.pm.ApplicationInfo;
import android.graphics.Bitmap;
import android.net.ConnectivityManager;
import android.net.Uri;
import android.net.wifi.WifiManager;
import android.os.Bundle;
import android.os.Build;
import android.provider.Settings;
import android.telephony.SmsManager;
import android.telephony.TelephonyManager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.webkit.DownloadListener;
import android.webkit.WebChromeClient;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.ImageButton;
import android.widget.ProgressBar;

import com.google.android.gms.ads.identifier.AdvertisingIdClient;
import com.google.android.gms.common.GooglePlayServicesNotAvailableException;
import com.google.android.gms.common.GooglePlayServicesRepairableException;

import org.cocos2dx.cpp.AndroidPlatform.ExecShell.SHELL_CMD;

import org.cocos2dx.lib.Cocos2dxHelper;

public class AndroidPlatform
{
    private static final String TAG = "AndroidPlatform";

    private static AppActivity activity;
//    public IRequestActionCallBack smsListener;
    public static Dialog webDialog;
    protected static WebView webView;
    private static ImageButton btnCloseWebview;
    public static AndroidPlatform instance;

    /**
     * SMS Broadcast Receiver
     */
    private static BroadcastReceiver smsReceiver;

//    GPSTracker gpsTracker;


    AndroidPlatform()
    {
        instance = this;
    }

    public static void setActivity(AppActivity _activity) {
        activity = _activity;
    }

    public void showNetworkSettingScreen() {
		/*
		 * vì wireless và data đều nằm ở ngoài setting
		 */
        Intent intent = new Intent(Settings.ACTION_SETTINGS);
        intent.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        activity.startActivity(intent);
    }


    /**
     * @param url
     *            : link muốn load
     * @param title
     *            : title của webview
     */

    public static void showWeb(final String url, final String title) {
        activity.runOnUiThread(new Runnable() {

            @SuppressWarnings("deprecation")
            @SuppressLint("SetJavaScriptEnabled")
            @Override
            public void run() {
                if (webDialog == null) {
                    LayoutInflater inflater = (LayoutInflater) activity
                            .getSystemService(Context.LAYOUT_INFLATER_SERVICE);
                    View layout = inflater.inflate(R.layout.web_view, null);
                    webView = (WebView) layout.findViewById(R.id.webview);
                    webView.getSettings().setJavaScriptEnabled(true);
                    webDialog = new Dialog(activity, R.style.WebViewTheme);
                    webDialog.addContentView(layout,
                            new LayoutParams(LayoutParams.FILL_PARENT,
                                    LayoutParams.WRAP_CONTENT));
                    webDialog.setContentView(layout);
                    ImageButton btnClose = (ImageButton) layout
                            .findViewById(R.id.btnClose);
                    btnClose.setOnClickListener(new OnClickListener() {

                        @Override
                        public void onClick(View v) {
                            webView.stopLoading();
                            webDialog.dismiss();
                        }
                    });
                    btnCloseWebview = btnClose;
                    final ProgressBar webProgressBar = (ProgressBar) layout
                            .findViewById(R.id.webview_progressbar);

                    // Add download listener.
                    webView.setDownloadListener(new DownloadListener() {
                        @Override
                        public void onDownloadStart(final String url,
                                                    String userAgent, String contentDisposition,
                                                    String mimetype, long contentLength) {
                            android.util.Log.d(TAG,
                                    "Start download from : {url=" + url
                                            + " ,userAgent=" + userAgent
                                            + " ,contentDisposition="
                                            + contentDisposition
                                            + " ,mimetype=" + mimetype
                                            + " ,contentLength="
                                            + contentLength + " }");
//                            if (url.endsWith(".apk") || url.endsWith(".APK")) {
//                                getApplicationManager().downloadAndSetupApp(
//                                        url, null, null);
//                            }
                        }
                    });

                    webView.setWebChromeClient(new WebChromeClient() {
                        @Override
                        public void onProgressChanged(WebView view,
                                                      int newProgress) {
                            super.onProgressChanged(view, newProgress);
                            webProgressBar.setProgress(newProgress * 100);
                        }
                    });
                    webView.setWebViewClient(new WebViewClient() {

                        @Override
                        public void onPageStarted(WebView view, String url,
                                                  Bitmap favicon) {
                            android.util.Log.d(TAG, "onPageStarted : url="
                                    + url);
//                            if (onPageStart != null) {
//                                onPageStart.actionPerformed(url);
//                            }
                            super.onPageStarted(view, url, favicon);
                            webProgressBar.setVisibility(View.VISIBLE);
                            webView.setEnabled(false);
                        }

                        @Override
                        public void onReceivedError(WebView view,
                                                    int errorCode, String description,
                                                    String failingUrl) {
                            super.onReceivedError(view, errorCode, description,
                                    failingUrl);
//							Toast.makeText(activity, description,
//									Toast.LENGTH_SHORT).show();
                            webProgressBar.setVisibility(View.GONE);
                            webView.setEnabled(true);
                        }

                        @Override
                        public void onPageFinished(WebView view, String url) {
                            super.onPageFinished(view, url);
                            webProgressBar.setVisibility(View.GONE);
                            webView.setEnabled(true);
                        }
                    });
                }
                if (url.startsWith("https://m.facebook.com/dialog/apprequests?")) {
                    btnCloseWebview.setVisibility(View.INVISIBLE);
                } else {
                    btnCloseWebview.setVisibility(View.VISIBLE);
                }
                webView.loadUrl(url);
                if (!webDialog.isShowing()) {
                    webDialog.show();
                }
            }
        });
    }


    public static void closeWebBrowser() {
        if (webDialog != null && webDialog.isShowing()) {
            webDialog.dismiss();
        }
    }

    public static void sendSMS(String phoneNumber, String message ) {

        String SENT = "SMS_SENT";
        String DELIVERED = "SMS_RECEIVED";
        Intent sentIntent = new Intent(SENT);
 PendingIntent sentPI = PendingIntent.getBroadcast(
       activity.getApplicationContext(), 0, sentIntent,
       PendingIntent.FLAG_UPDATE_CURRENT);

        Intent deliveryIntent = new Intent(DELIVERED);
        deliveryIntent.putExtra("address", phoneNumber);
        deliveryIntent.putExtra("sms_body", message);
        PendingIntent deliverPI = PendingIntent.getBroadcast( activity.getApplicationContext(), 0, deliveryIntent, PendingIntent.FLAG_UPDATE_CURRENT);

        // ---when the SMS has been sent---
        activity.registerReceiver(new BroadcastReceiver() {
            @Override
            public void onReceive(Context arg0, Intent arg1) {
                switch (getResultCode()) {
                case Activity.RESULT_OK:
                    break;
                case SmsManager.RESULT_ERROR_GENERIC_FAILURE:
                case SmsManager.RESULT_ERROR_NO_SERVICE:
                case SmsManager.RESULT_ERROR_NULL_PDU:
                case SmsManager.RESULT_ERROR_RADIO_OFF:
                    activity.runOnGLThread(new Runnable()
                    {
                        @Override
                        public void run() {

                            nativeOnSMSFail();
                        }
                    });
//                    onSMSFail(null);
                    break;
                }
            }
        }, new IntentFilter(SENT));

        // ---when the SMS has been delivered---
        activity.registerReceiver(smsReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context arg0, Intent arg1) {
                switch (getResultCode()) {
                case Activity.RESULT_OK:
                    Bundle b = arg1.getExtras();
                    final String phone_number = b.getString("address");
                    final String sms_body = b.getString("sms_body");
                    activity.runOnGLThread(new Runnable()
                    {
                        @Override
                        public void run() {

                            nativeOnSMSSucceed(phone_number, sms_body);
                        }
                    });
//                    onSMSSucceed(null);
                    break;
                case Activity.RESULT_CANCELED:
//                    onSMSFail(null);
                    activity.runOnGLThread(new Runnable()
                    {
                        @Override
                        public void run() {

                            nativeOnSMSFail();
                        }
                    });
                    break;
                }
            }
        }, new IntentFilter(DELIVERED));


        SmsManager smsManager = SmsManager.getDefault();
        smsManager.sendTextMessage(phoneNumber, null, message, sentPI, deliverPI);
    }

    public static void showSMSComposer(String phoneNumber, String message)
    {
        String SENT = "SMS_SENT";
        String DELIVERED = "SMS_RECEIVED";

        // ---when the SMS has been sent---
        activity.registerReceiver(new BroadcastReceiver() {
            @Override
            public void onReceive(Context arg0, Intent arg1) {
                switch (getResultCode()) {
                case Activity.RESULT_OK:
                    break;
                case SmsManager.RESULT_ERROR_GENERIC_FAILURE:
                case SmsManager.RESULT_ERROR_NO_SERVICE:
                case SmsManager.RESULT_ERROR_NULL_PDU:
                case SmsManager.RESULT_ERROR_RADIO_OFF:
                    activity.runOnGLThread(new Runnable()
                    {
                        @Override
                        public void run() {

                            nativeOnSMSFail();
                        }
                    });
//                    onSMSFail(null);
                    break;
                }
            }
        }, new IntentFilter(SENT));

        // ---when the SMS has been delivered---
        activity.registerReceiver(smsReceiver = new BroadcastReceiver() {
            @Override
            public void onReceive(Context arg0, Intent arg1) {
                switch (getResultCode()) {
                case Activity.RESULT_OK:
                    final String phone_number = arg1.getStringExtra("address");
                    final String sms_body = arg1.getStringExtra("sms_body");
                    activity.runOnGLThread(new Runnable()
                    {
                        @Override
                        public void run() {

                            nativeOnSMSSucceed(phone_number, sms_body);
                        }
                    });
//                    onSMSSucceed(null);
                    break;
                case Activity.RESULT_CANCELED:
//                    onSMSFail(null);
                    activity.runOnGLThread(new Runnable()
                    {
                        @Override
                        public void run() {

                            nativeOnSMSFail();
                        }
                    });
                    break;
                }
            }
        }, new IntentFilter(DELIVERED));

        Intent sendIntent = new Intent(Intent.ACTION_VIEW);
        sendIntent.putExtra("address", phoneNumber);
        sendIntent.putExtra("sms_body", message);
        sendIntent.setType("vnd.android-dir/mms-sms");
        activity.startActivity(sendIntent);

    }

    public static void makeCall(String phoneNumber) {
        try {
            PackageManager pm = activity.getPackageManager();
            boolean hasTelephony = pm
                    .hasSystemFeature(PackageManager.FEATURE_TELEPHONY);
            TelephonyManager telMgr = (TelephonyManager) activity.getSystemService(Context.TELEPHONY_SERVICE);

            if (!hasTelephony
                    || telMgr.getSimState() != TelephonyManager.SIM_STATE_READY) {
                return;
            }

            Intent callIntent = new Intent(Intent.ACTION_CALL);
            callIntent.setData(Uri.parse("tel:" + phoneNumber));
            activity.startActivity(callIntent);
        } catch (Exception e) {

        }
    }

    public static String getAppName()
    {
        ApplicationInfo ai;
        PackageManager pm = activity.getPackageManager();
        try {
            ai = pm.getApplicationInfo(activity.getApplicationInfo().packageName, 0);
        } catch (final NameNotFoundException e) {
            ai = null;
        }
        final String applicationName = (String) (ai != null ? pm.getApplicationLabel(ai) : "(unknown)");

        return applicationName;
    }

    public static String getPhoneName()
    {
        return Cocos2dxHelper.getDeviceModel();
    }

    public static String getVersion()
    {
        return Cocos2dxHelper.getVersion();
    }

    public static String getOsVersion()
    {
        return Build.VERSION.RELEASE;
    }

    public static String getNet()
    {
        final ConnectivityManager connMgr = (ConnectivityManager) activity
                .getSystemService(Context.CONNECTIVITY_SERVICE);

        final android.net.NetworkInfo wifi = connMgr.getNetworkInfo(ConnectivityManager.TYPE_WIFI);
        final android.net.NetworkInfo mobile = connMgr.getNetworkInfo(ConnectivityManager.TYPE_MOBILE);

        if (wifi != null && wifi.isAvailable() && wifi.isConnected()) {
            return "wifi";
        } else if (mobile != null && mobile.isAvailable() && mobile.isConnected()) {
            TelephonyManager telManager = (TelephonyManager) activity.getSystemService(Context.TELEPHONY_SERVICE);
            if (telManager != null) {
                int cType = telManager.getNetworkType();
                switch (cType) {
                case 1:
                    return "gprs";
                case 2:
                    return "edge";
                case 3:
                    return "umts";
                case 8:
                    return "hsdpa";
                case 9:
                    return "hsupa";
                case 10:
                    return "hspa";
                default:
                    return "unknown";

                }
            }
        }
        return "none";
    }

    public static String getCarrier()
    {
        TelephonyManager manager = (TelephonyManager) activity.getSystemService(Context.TELEPHONY_SERVICE);
        String carrierName = manager.getNetworkOperatorName();
        if (carrierName == null || carrierName == "") {
            return "UnknowNetwork";
        }
        return carrierName;
    }

    public static String getMacAddress() {
        try {
            WifiManager wm = (WifiManager) activity.getSystemService(Context.WIFI_SERVICE);
            String result = wm.getConnectionInfo().getMacAddress();
            return result;
        } catch (Exception e) {
            // TODO: handle exception
            e.printStackTrace();
            return "0";
        }
    }

    public static String getAdvertisingID() {
        AdvertisingIdClient.Info adInfo = null;
        try {
            try {
                adInfo = AdvertisingIdClient.getAdvertisingIdInfo(AppActivity.instance);
            } catch (IllegalStateException e) {
                e.printStackTrace();
            } catch (GooglePlayServicesRepairableException e) {
                e.printStackTrace();
            }
        } catch (IOException e) {
            e.printStackTrace();
        } catch (GooglePlayServicesNotAvailableException e) {
            e.printStackTrace();
        }

        String AdId = adInfo.getId();
        return AdId;
    }

    public static String getAndroidID()
    {
        return Settings.Secure.getString(AppActivity.instance.getContentResolver(), Settings.Secure.ANDROID_ID);
    }

    public static String getSimSeri()
    {
        String result = "0";
        TelephonyManager tm = (TelephonyManager) AppActivity.instance.getApplicationContext().getSystemService(Context.TELEPHONY_SERVICE);
        try {
            result = tm.getSimSerialNumber();
        } catch (Exception e) {
            e.printStackTrace();
        }
        if (result == null) {
            result = "0";
        }
        return result;
    }

    public static String getFingerPrint()
    {
        return "";
    }

    public static String getSubscriberID()
    {
        TelephonyManager tm = (TelephonyManager) AppActivity.instance.getApplicationContext().getSystemService(Context.TELEPHONY_SERVICE);
        String result = tm.getDeviceId();
        result = result != "" ? result : "0";
        return result;
    }

    public static void GGSignIn() {
        GoogleGameServices.getManager().login(
                new IActionListener() {

                    @Override
                    public void actionPerformed(Object o) {
                        String accessToken = (String) o;
                        GoogleGameServices.getManager()
                                .setAccessToken(accessToken);
                        doLoginWithGGS(accessToken);
                    }
                });
    }

    public static void doLoginWithGGS(final String accessToken) {
        activity.runOnGLThread(new Runnable()
        {
            @Override
            public void run() {
                nativeOnGGSignInCompleted(accessToken);
            }
        });
    }

    public static void GGSignOut() {
        GoogleGameServices.getManager().logOut(new IActionListener() {
            @Override
            public void actionPerformed(Object arg0) {

            }
        });
    }

    public static boolean IsGGConnected() {
        return GoogleGameServices.getManager().isSignedIn();
    }

    public static String GetGGToken() {
        return GoogleGameServices.getManager().getAccessToken();
    }

    public static void DismissSplash() { AppActivity.instance.dismissLoader();}

    public static String[] GetAppEmail()
    {
        String[] packageName = Constants.LIST_APP_EMAIL_PACKAGE_ANDROID.split(",");
        String[] packageName_App = Constants.LIST_APP_EMAIL_NAMEAPP_ANDROID.split(",");
        ArrayList<String> nameapp = new ArrayList<String>();
        for (int i = 0; i < packageName.length; i++) {
            Intent LaunchIntent = activity.getPackageManager().getLaunchIntentForPackage(packageName[i]);
            if (LaunchIntent != null) {
                nameapp.add(packageName_App[i]);
            }
        }
        return nameapp.toArray(new String[nameapp.size()]);
    }

    public static void setCLipboardText(String text)
    {
        final String temp_text = text;

        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                int sdk = android.os.Build.VERSION.SDK_INT;
                if (sdk < android.os.Build.VERSION_CODES.HONEYCOMB) {
                    android.text.ClipboardManager clipboard = (android.text.ClipboardManager) activity
                            .getSystemService(Context.CLIPBOARD_SERVICE);
                    clipboard.setText(temp_text);
                } else {
                    android.content.ClipboardManager clipboard = (android.content.ClipboardManager) activity
                            .getSystemService(Context.CLIPBOARD_SERVICE);
                    android.content.ClipData clip = android.content.ClipData.newPlainText(temp_text, temp_text);
                    clipboard.setPrimaryClip(clip);
                }
            }
        });
    }


    public static String getClipboardText()
    {
        activity.runOnUiThread(new Runnable() {
            @Override
            public void run() {
                String text = "";
                int sdk = android.os.Build.VERSION.SDK_INT;
                if (sdk < android.os.Build.VERSION_CODES.HONEYCOMB) {
                    android.text.ClipboardManager clipboard = (android.text.ClipboardManager) activity
                            .getSystemService(Context.CLIPBOARD_SERVICE);
                    text = clipboard.getText().toString();
                } else {
                    ClipboardManager clipboardManager = (ClipboardManager) activity
                            .getSystemService(Context.CLIPBOARD_SERVICE);
                    ClipData clip = clipboardManager.getPrimaryClip();
                    if (clip != null) {
                        ClipData.Item item = clip.getItemAt(0);
                        text = item.getText().toString();
                    }
                }

                nativeReturnClipboardText(text);
            }
        });
        return "aaaaa";
    }

    static int root_device;

    public static int getRoot()
    {
        if (root_device == -1) {
            root_device = (new Root()).isDeviceRooted() ? 1 : 0;
        }
        return root_device;
    }

   public static class Root {

        public boolean isDeviceRooted() {
            if (checkRootMethod1()) {
                return true;
            }
            if (checkRootMethod2()) {
                return true;
            }
            if (checkRootMethod3()) {
                return true;
            }
            return false;
        }

        public boolean checkRootMethod1() {
            String buildTags = android.os.Build.TAGS;

            if (buildTags != null && buildTags.contains("test-keys")) {
                return true;
            }
            return false;
        }

        public boolean checkRootMethod2() {
            try {
                File file = new File("/system/app/Superuser.apk");
                if (file.exists()) {
                    return true;
                }
            } catch (Exception e) {
            }

            return false;
        }

        public boolean checkRootMethod3() {
            if (new ExecShell().executeCommand(SHELL_CMD.check_su_binary) != null) {
                return true;
            } else {
                return false;
            }
        }
    }

    public static class ExecShell {

        // private static String LOG_TAG = ExecShell.class.getName();

        public static enum SHELL_CMD {
            check_su_binary(new String[] { "/system/xbin/which", "su" }),;

            String[] command;

            SHELL_CMD(String[] command) {
                this.command = command;
            }
        }

        public ArrayList<String> executeCommand(SHELL_CMD shellCmd) {
            String line = null;
            ArrayList<String> fullResponse = new ArrayList<String>();
            Process localProcess = null;

            try {
                localProcess = Runtime.getRuntime().exec(shellCmd.command);
            } catch (Exception e) {
                return null;
                // e.printStackTrace();
            }

            // BufferedWriter out = new BufferedWriter(new OutputStreamWriter(
            // localProcess.getOutputStream()));
            BufferedReader in = new BufferedReader(new InputStreamReader(localProcess.getInputStream()));

            try {
                while ((line = in.readLine()) != null) {
                    // Log.d(LOG_TAG, "--> Line received: " + line);
                    fullResponse.add(line);
                }
            } catch (Exception e) {
                // e.printStackTrace();
            }

            // Log.d(LOG_TAG, "--> Full response was: " + fullResponse);

            return fullResponse;
        }

    }

    public static void OpenAppName(String appName)
    {
        String[] packageName = Constants.LIST_APP_EMAIL_PACKAGE_ANDROID.split(",");
        String[] packageName_App = Constants.LIST_APP_EMAIL_NAMEAPP_ANDROID.split(",");
        for (int i = 0; i < packageName_App.length; i++) {
            if (packageName_App[i].equalsIgnoreCase(appName)) {
                Intent LaunchIntent = activity.getPackageManager().getLaunchIntentForPackage(packageName[i]);
                if (LaunchIntent != null) {
                    activity.startActivity(LaunchIntent);
                }
            }
        }
    }

    public static native void nativeOnSMSSucceed(String phone_number, String sms_body);
    public static native void nativeOnSMSFail();
    public static native void nativeReturnClipboardText(String text);
    public static native void nativeOnGGSignInCompleted(String token);

}